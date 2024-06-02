#include "cred_storage_writer.hpp"

#include <kos/trace.h>

#include <fstream>

namespace nas {

CredStorageWriter& CredStorageWriter::Get()
{
    static CredStorageWriter instance;

    return instance;
}

Retcode CredStorageWriter::AddAuthEntry(const std::string& username, const std::string& password) noexcept
try
{
    if (const auto rc{GetPasswordHash(password)}; rc != rcOk)
    {
        ERROR(CRED_STORAGE_READER, "Could not get hash for {}", password.c_str());
        return rcFail;
    }

    std::ofstream hashFile{username, std::ios::out | std::ios::trunc};
    if (!hashFile.is_open())
    {
        ERROR(CRED_STORAGE_WRITER, "Could not open file: %s", username.c_str());
        return rcInternalError;
    }

    hashFile << passHash;
    hashFile.flush();

    return rcOk;
}
catch (const std::exception& e)
{
    ERROR(CRED_STORAGE_WRITER, "Could not add authentication entry: %s", e.what());
    return rcInternalError;
}

} // namespace nas
