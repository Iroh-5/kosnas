#pragma once

#include <rtl/retcode.h>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

namespace nas {

class FsManager
{
    FsManager(const FsManager& other) = delete;
    FsManager(FsManager&& other)      = delete;

    FsManager& operator ==(const FsManager& other) = delete;
    FsManager& operator ==(FsManager&& other)      = delete;

public:
    static FsManager& Get();

    Retcode Init(const fs::path& dirPath, std::size_t maxSpacePerPart = 1'000'000) noexcept;

    Retcode InitiateFileReceit(const fs::path& filePath, std::size_t fileSize) noexcept;
    Retcode ReceiveFileData(const std::vector<std::uint8_t>& data) noexcept;

    Retcode InitiateFileTransmission(const fs::path& filePath, std::size_t& fileSize) noexcept;
    Retcode TransmitFileData(std::vector<std::uint8_t>& data, std::size_t dataSize) noexcept;

    Retcode Deinit() noexcept;

private:
    enum class State
    {
        IDLE,
        FILE_RECEIT,
        FILE_TRANSMISSION
    };

private:
    FsManager() = default;

    fs::path GetRealFilePath(const fs::path& path);
    void     FinalizeFileOperation();

private:
    State m_currentState{State::IDLE};

    fs::path m_rootPath;

    std::fstream m_currentFile;
    std::size_t  m_currentFileSize;

    std::size_t m_maxSpacePerPart;
    std::unordered_map<std::string, std::size_t> m_partUsedSpace;
};

} // namespace nas

