#include "fs_test_common.hpp"

#include <rtl/assert.h>

#include <kos/trace.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace test {

namespace fs = std::filesystem;

std::string GetTmpDirPathStr()
{
    static auto id{0};

    return "tmp" + std::to_string(id++) + "/";
}

std::string GetTmpFilePathStr()
{
    static auto id{0};

    return "file" + std::to_string(id++);
}

std::string GetFullFilePathStr(const std::string& dirName, const std::string& fileName)
{
    return fs::path{fs::path{dirName} / fs::path{fileName}}.string();
}

bool CheckFileContent(const fs::path& path, const std::string& fileContentToCmp)
{
    std::ifstream ifs{path};

    rtl_assert(ifs.is_open());

    const auto fileSize{fs::file_size(path)};

    std::string fileContentStr;
    fileContentStr.resize(fileSize);
    ifs.read(
        reinterpret_cast<decltype(ifs)::char_type*>(fileContentStr.data()),
        static_cast<std::streamsize>(fileSize));

    if (fileContentStr != fileContentToCmp)
    {
        ERROR(TEST_COMMON,
            "File content does not match. Expected: %s (%u), got: %s (%u)",
            fileContentToCmp.c_str(),
            static_cast<unsigned>(fileContentToCmp.size()),
            fileContentStr.c_str(),
            static_cast<unsigned>(fileContentStr.size()));

        return false;
    }

    return true;
}

} // namespace test
