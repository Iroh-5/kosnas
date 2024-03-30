#pragma once

#include <rtl/retcode.h>

#include <cstdint>
#include <filesystem>
#include <fstream>

namespace nas {

namespace fs = std::filesystem;

class FsBase
{
    FsBase(const FsBase& other) = delete;
    FsBase(FsBase&& other)      = delete;

    FsBase& operator ==(const FsBase& other) = delete;
    FsBase& operator ==(FsBase&& other)      = delete;

public:
    Retcode Init(const fs::path& dirPath) noexcept;
    Retcode Deinit() noexcept;

protected:
    enum class State
    {
        IDLE,
        FILE_RECEIT,
        FILE_TRANSMISSION
    };

protected:
    FsBase() = default;

    fs::path GetFullFilePath(const fs::path& path);
    void     FinalizeFileOperation();

protected:
    State m_currentState{State::IDLE};

    fs::path m_rootPath;

    std::fstream m_currentFile;
    std::size_t  m_currentFileSize;
};

} // namespace nas
