#include "fs_writer.hpp"

#include <fmt/format.h>

#include <kos/trace.h>

#include <rtl/assert.h>

#include <algorithm>
#include <stdexcept>

namespace nas {

FsWriter& FsWriter::Get()
{
    static FsWriter instance;
    return instance;
}

Retcode FsWriter::Init(const fs::path& dirPath, std::size_t maxSpacePerPart) noexcept
{
    rtl_assert(maxSpacePerPart > 0u);
    rtl_assert(!dirPath.has_filename());

    std::error_code ec;
    if (!fs::create_directory(dirPath, ec))
    {
        ERROR(FS_WRITER, "Could not create directory %s", dirPath.c_str());
        return rcFail;
    }

    if (ec)
    {
        ERROR(FS_WRITER, "fs::create_directory failed: %s", ec.message().c_str());
        return rcFail;
    }

    m_maxSpacePerPart = maxSpacePerPart;

    return FsBase::Init(dirPath);
}

Retcode FsWriter::InitiateFileReceit(const fs::path& filePath, std::size_t fileSize) noexcept
try
{
    rtl_assert(!m_rootPath.empty());

    if (m_currentState != State::IDLE)
    {
        DEBUG(FS_WRITER, "%s was called in invalid state (%d)", __func__, static_cast<int>(m_currentState));
    }

    if (m_partUsedSpace[m_rootPath.string()] + fileSize > m_maxSpacePerPart)
    {
        ERROR(FS_WRITER, "Could not initiate file receit: Max space use has been reached");
        return rcOutOfMemory;
    }

    m_currentFile.open(GetFullFilePath(filePath), std::ios::out | std::ios::app);
    if (!m_currentFile.is_open())
    {
        ERROR(FS_WRITER, "Failed to create file: %s", filePath.c_str());
        return rcFail;
    }

    m_currentFileSize = fileSize;
    m_currentState    = State::FILE_RECEIT;

    return rcOk;
}
catch (const std::exception& e)
{
    ERROR(FS_WRITER, "Could not initiate file receit: %s", e.what());
    return rcFail;
}

Retcode FsWriter::ReceiveFileData(const std::vector<std::uint8_t>& data) noexcept
try
{
    if (m_currentState != State::FILE_RECEIT)
    {
        DEBUG(FS_WRITER, "%s was called in invalid state (%d)", __func__, static_cast<int>(m_currentState));
        return rcInvalidOperation;
    }

    rtl_assert(m_currentFile.is_open());

    m_currentFile.write(
        reinterpret_cast<const decltype(m_currentFile)::char_type*>(data.data()),
        static_cast<std::streamsize>(std::min(data.size(), m_currentFileSize)));

    if (data.size() > m_currentFileSize)
    {
        WARN(FS_WRITER, "Got more data than expected");
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
    ERROR(FS_WRITER, "Failed to receive file data: %s", e.what());
    return rcFail;
}

} // namespace nas
