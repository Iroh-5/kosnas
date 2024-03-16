#include "fs_manager.hpp"

#include <fmt/format.h>

#include <kos/trace.h>

#include <rtl/assert.h>

#include <algorithm>
#include <stdexcept>

namespace nas {

FsManager& FsManager::Get()
{
    static FsManager instance;
    return instance;
}

Retcode FsManager::Init(const fs::path& dirPath, std::size_t maxSpacePerPart) noexcept
try
{
    m_rootPath        = dirPath;
    m_maxSpacePerPart = maxSpacePerPart;

    std::error_code ec;
    if (!fs::create_directory(dirPath, ec))
    {
        throw std::invalid_argument{
            fmt::format(
                "Failed to create directory at path: {}", dirPath.string() == "" ? "<empty>" : dirPath.string())};
    }

    if (ec)
    {
        throw std::runtime_error{fmt::format("fs::create_directory failed: {}", ec.message())};
    }

    return rcOk;
}
catch (const std::exception& e)
{
    ERROR(FS_MANAGER, "Initialization failed: %s", e.what());
    return rcFail;
}

Retcode FsManager::InitiateFileReceit(const fs::path& filePath, std::size_t fileSize) noexcept
try
{
    rtl_assert(!m_rootPath.empty());

    if (m_currentState != State::IDLE)
    {
        DEBUG(FS_MANAGER, "%s was called in invalid state (%d)", __func__, static_cast<int>(m_currentState));
    }

    if (m_partUsedSpace[m_rootPath.string()] + fileSize > m_maxSpacePerPart)
    {
        ERROR(FS_MANAGER, "Could not initiate file receit: Max space use has been reached");
        return rcOutOfMemory;
    }

    m_currentFile.open(GetRealFilePath(filePath), std::ios::out | std::ios::app);
    if (!m_currentFile.is_open())
    {
        ERROR(FS_MANAGER, "Failed to create file: %s", filePath.c_str());
        return rcFail;
    }

    m_currentFileSize = fileSize;
    m_currentState    = State::FILE_RECEIT;

    return rcOk;
}
catch (const std::exception& e)
{
    ERROR(FS_MANAGER, "Could not initiate file receit: %s", e.what());
    return rcFail;
}

Retcode FsManager::ReceiveFileData(const std::vector<std::uint8_t>& data) noexcept
try
{
    if (m_currentState != State::FILE_RECEIT)
    {
        DEBUG(FS_MANAGER, "%s was called in invalid state (%d)", __func__, static_cast<int>(m_currentState));
        return rcInvalidOperation;
    }

    rtl_assert(m_currentFile.is_open());

    m_currentFile.write(
        reinterpret_cast<const decltype(m_currentFile)::char_type*>(data.data()),
        static_cast<std::streamsize>(std::min(data.size(), m_currentFileSize)));

    if (data.size() > m_currentFileSize)
    {
        WARN(FS_MANAGER, "Got more data than expected");
    }

    m_currentFileSize -= std::min(data.size(), m_currentFileSize);
    if (m_currentFileSize == 0)
    {
        m_currentState = State::IDLE;
        FinalizeFileOperation();
    }

    return rcOk;
}
catch (const std::exception& e)
{
    ERROR(FS_MANAGER, "Failed to receive file data: %s", e.what());
    return rcFail;
}

Retcode FsManager::InitiateFileTransmission(const fs::path& filePath, std::size_t& fileSize) noexcept
try
{
    rtl_assert(!m_rootPath.empty());

    if (m_currentState != State::IDLE)
    {
        DEBUG(FS_MANAGER, "%s was called in invalid state (%d)", __func__, static_cast<int>(m_currentState));
        FinalizeFileOperation();
    }

    m_currentFile.open(GetRealFilePath(filePath), std::ios::in);
    if (!m_currentFile.is_open())
    {
        ERROR(FS_MANAGER, "Failed to open file: %s", filePath.c_str());
        return rcFail;
    }

    m_currentFileSize = fs::file_size(GetRealFilePath(filePath));
    m_currentState    = State::FILE_TRANSMISSION;

    fileSize = m_currentFileSize;

    return rcOk;
}
catch (const std::exception& e)
{
    ERROR(FS_MANAGER, "Failed to initiate file transmission: %s", e.what());
    return rcFail;
}

Retcode FsManager::TransmitFileData(std::vector<std::uint8_t>& data, std::size_t dataSize) noexcept
try
{
    if (m_currentState != State::FILE_TRANSMISSION)
    {
        DEBUG(FS_MANAGER, "%s was called in invalid state (%d)", __func__, static_cast<int>(m_currentState));
        return rcInvalidOperation;
    }

    if (data.size() < dataSize)
    {
        ERROR(FS_MANAGER, "Passed array is too small. Array size: %zu, requested data chunk size: %zu",
            data.size(), dataSize);
        return rcInvalidArgument;
    }

    rtl_assert(m_currentFile.is_open());

    m_currentFile.read(
        reinterpret_cast<decltype(m_currentFile)::char_type*>(data.data()),
        static_cast<std::streamsize>(dataSize));

    if (dataSize > m_currentFileSize)
    {
        WARN(FS_MANAGER, "Requested for more data than stored");
    }

    m_currentFileSize -= std::min(dataSize, m_currentFileSize);
    if (m_currentFileSize == 0)
    {
        m_currentState = State::IDLE;
        FinalizeFileOperation();
    }

    return rcOk;
}
catch (const std::exception& e)
{
    ERROR(FS_MANAGER, "Failed to transmit file data: %s", e.what());
    return rcFail;
}

Retcode FsManager::Deinit() noexcept
try
{
    FinalizeFileOperation();

    m_rootPath     = fs::path{};
    m_currentState = State::IDLE;

    return rcOk;
}
catch (const std::exception& e)
{
    ERROR(FS_MANAGER, "Deinitialization failed: %s", e.what());
    return rcFail;
}

fs::path FsManager::GetRealFilePath(const fs::path& path)
{
    return m_rootPath / path;
}

void FsManager::FinalizeFileOperation()
{
    m_currentFile.close();
}

} // namespace nas

