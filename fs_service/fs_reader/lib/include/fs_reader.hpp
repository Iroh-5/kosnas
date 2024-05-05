#pragma once

#include "fs_base.hpp"

#include <rtl/retcode.h>

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>

namespace nas {

class FsReader final
    : public FsBase
{
public:
    static FsReader& Get();

    Retcode Init(const fs::path& dirPath) noexcept;

    Retcode InitiateFileTransmission(const fs::path& filePath, std::size_t& fileSize) noexcept;
    Retcode TransmitFileData(std::string& data, std::size_t dataSize) noexcept;

private:
    FsReader() = default;
};

} // namespace nas
