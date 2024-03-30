#include "fs_reader.hpp"

#include <fmt/format.h>

#include <kos/trace.h>

#include <rtl/assert.h>

#include <algorithm>
#include <stdexcept>

namespace nas {

FsReader& FsReader::Get()
{
    static FsReader instance;
    return instance;
}

Retcode FsReader::Init(const fs::path& dirPath) noexcept
{
    rtl_assert(!dirPath.has_filename());

    if (!fs::exists(dirPath))
    {
        ERROR(FS_READER, "Directory %s does not exist", dirPath.c_str());
        return rcFail;
    }

    return FsBase::Init(dirPath);
}

Retcode FsReader::InitiateFileTransmission(const fs::path& filePath, std::size_t& fileSize) noexcept
try
{
    rtl_assert(!m_rootPath.empty());

    if (m_currentState != State::IDLE)
    {
        DEBUG(FS_READER, "%s was called in invalid state (%d)", __func__, static_cast<int>(m_currentState));
        FinalizeFileOperation();
    }

    m_currentFile.open(GetFullFilePath(filePath), std::ios::in);
    if (!m_currentFile.is_open())
    {
        ERROR(FS_READER, "Failed to open file: %s", filePath.c_str());
        return rcFail;
    }

    m_currentFileSize = fs::file_size(GetFullFilePath(filePath)) - 1;
    m_currentState    = State::FILE_TRANSMISSION;

    fileSize = m_currentFileSize;

    return rcOk;
}
catch (const std::exception& e)
{
    ERROR(FS_READER, "Failed to initiate file transmission: %s", e.what());
    return rcFail;
}

Retcode FsReader::TransmitFileData(std::vector<std::uint8_t>& data, std::size_t dataSize) noexcept
try
{
    if (m_currentState != State::FILE_TRANSMISSION)
    {
        DEBUG(FS_READER, "%s was called in invalid state (%d)", __func__, static_cast<int>(m_currentState));
        return rcInvalidOperation;
    }

    if (data.size() < dataSize)
    {
        ERROR(FS_READER, "Passed array is too small. Array size: %zu, requested data chunk size: %zu",
            data.size(), dataSize);
        return rcInvalidArgument;
    }

    rtl_assert(m_currentFile.is_open());

    m_currentFile.read(
        reinterpret_cast<decltype(m_currentFile)::char_type*>(data.data()),
        static_cast<std::streamsize>(dataSize));

    if (dataSize > m_currentFileSize)
    {
        WARN(FS_READER, "Requested for more data than stored");
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
    ERROR(FS_READER, "Failed to transmit file data: %s", e.what());
    return rcFail;
}

} // namespace nas
