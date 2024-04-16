#pragma once

#include <rtl/retcode.h>

#include <string>

namespace nas {

class CredStorageWriter
{
public:
    static CredStorageWriter& Get();

    Retcode AddAuthEntry(const std::string& username, const std::string& passHash) noexcept;

private:
    CredStorageWriter() = default;
};

} // namespace nas
