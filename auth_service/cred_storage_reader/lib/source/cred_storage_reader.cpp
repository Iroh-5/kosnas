#include "cred_storage_reader.hpp"

#include <kos/trace.h>

#include <rtl/assert.h>

#include <filesystem>
#include <fstream>

namespace nas {

namespace fs = std::filesystem;

CredStorageReader& CredStorageReader::Get()
{
    static CredStorageReader instance;

    return instance;
}

Retcode CredStorageReader::Authenticate(const std::string& username,  const std::string& password) noexcept
try
{
    if (const auto rc{GetPasswordHash(password)}; rc != rcOk)
    {
        ERROR(CRED_STORAGE_READER, "Could not get hash for {}", password.c_str());
        return rcFail;
    }

    const auto hashFilePath{fs::path{username}};

    if (!fs::exists(hashFilePath))
    {
        ERROR(CRED_STORAGE_READER, "User %s is not in storage", username.c_str());
        return rcResourceNotFound;
    }

    const auto hashFileSize{fs::file_size(hashFilePath)};

    std::ifstream hashFile{hashFilePath};
    if (!hashFile.is_open())
    {
        ERROR(CRED_STORAGE_READER, "Could not open file: %s", username.c_str());
        return rcInternalError;
    }

    std::string storedHash;
    storedHash.resize(hashFileSize);

    hashFile.read(
        reinterpret_cast<decltype(hashFile)::char_type*>(storedHash.data()),
        static_cast<std::streamsize>(hashFileSize));

    if (storedHash != passHash)
    {
        ERROR(CRED_STORAGE_READER, "Passed wrong password hash for user %s", username.c_str());
        return rcFail;
    }

    return rcOk;
}
catch (const std::exception& e)
{
    ERROR(CRED_STORAGE_READER, "Failed to authenticate use %s: %s", username.c_str(), e.what());
    return rcInternalError;
}

} // namespace nas
