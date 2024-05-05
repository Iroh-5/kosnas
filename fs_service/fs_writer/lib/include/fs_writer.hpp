#pragma once

#include "fs_base.hpp"

#include <rtl/retcode.h>

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>

namespace nas {

class FsWriter final
    : public FsBase
{
public:
    static FsWriter& Get();

    Retcode Init(const fs::path& dirPath, std::size_t maxSpacePerPart = 1'000'000) noexcept;

    Retcode InitiateFileReceit(const fs::path& filePath, std::size_t fileSize) noexcept;
    Retcode ReceiveFileData(const std::string& data) noexcept;

private:
    FsWriter() = default;

private:
    std::size_t m_maxSpacePerPart;
    std::unordered_map<std::string, std::size_t> m_partUsedSpace;
};

} // namespace nas

