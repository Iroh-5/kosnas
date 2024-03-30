#include "fs_base.hpp"

#include <fmt/format.h>

#include <kos/trace.h>

#include <stdexcept>

namespace nas {

Retcode FsBase::Init(const fs::path& dirPath) noexcept
try
{
    m_rootPath = dirPath;

    return rcOk;
}
catch (const std::exception& e)
{
    ERROR(FS_BASE, "Initialization failed: %s", e.what());
    return rcFail;
}

Retcode FsBase::Deinit() noexcept
try
{
    FinalizeFileOperation();

    m_rootPath     = fs::path{};
    m_currentState = State::IDLE;

    return rcOk;
}
catch (const std::exception& e)
{
    ERROR(FS_BASE, "Deinitialization failed: %s", e.what());
    return rcFail;
}

fs::path FsBase::GetFullFilePath(const fs::path& path)
{
    return m_rootPath / path;
}

void FsBase::FinalizeFileOperation()
{
    m_currentFile.close();
}

} // namespace nas
