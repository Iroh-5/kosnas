#pragma once

#include <rtl/retcode.h>

#include <string>

namespace nas {

class CredStorageReader
{
public:
    static CredStorageReader& Get();

    Retcode Authenticate(const std::string& username, const std::string& passHash) noexcept;

private:
    CredStorageReader() = default;
};

} // namespace nas
