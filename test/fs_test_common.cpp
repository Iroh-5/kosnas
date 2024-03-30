#include "fs_test_common.hpp"

#include <rtl/assert.h>

#include <fstream>
#include <sstream>

namespace test {

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

    std::stringstream fileContent;
    fileContent << ifs.rdbuf();

    return fileContent.str() == fileContentToCmp;
}

} // namespace test
