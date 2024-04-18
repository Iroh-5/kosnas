#pragma once

#include <string>

namespace nas {

class FsServiceProxy
{
public:
    Retcode SendFile(const std::string& filePath, const std::string& fileContent) noexcept;
    Retcode GetFile (const std::string& filePath, std::string& fileContent) noexcept;
};

} // namespace nas
