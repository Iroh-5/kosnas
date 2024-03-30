#include <filesystem>
#include <string>

namespace test {

namespace fs = std::filesystem;

std::string GetTmpDirPathStr();
std::string GetTmpFilePathStr();
std::string GetFullFilePathStr(const std::string& dirName, const std::string& fileName);

bool CheckFileContent(const fs::path& path, const std::string& fileContentToCmp);

} // namespace test
