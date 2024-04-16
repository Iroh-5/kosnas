#include "fs_test_common.hpp"
#include "cred_storage_writer/lib/include/cred_storage_writer.hpp"

#include <gtest/gtest.h>

#include <filesystem>

namespace fs = std::filesystem;

namespace {

const std::string UserName{"creds/SomeUser2"};
const std::string NonExistingUserName{"creds/SomeUser3"};

} // namespace

TEST(AddAuthEntry, OverwriteExistingEntry_Success)
{
    auto& credStorageWriter{nas::CredStorageWriter::Get()};

    ASSERT_TRUE(test::CheckFileContent(UserName, "password\n"));

    ASSERT_EQ(rcOk, credStorageWriter.AddAuthEntry(UserName, "123456789"));

    ASSERT_TRUE(test::CheckFileContent(UserName, "123456789"));
}

TEST(AddAuthEntry, AddNonExistingEntry_Success)
{
    auto& credStorageWriter{nas::CredStorageWriter::Get()};

    ASSERT_FALSE(fs::exists(NonExistingUserName));

    ASSERT_EQ(rcOk, credStorageWriter.AddAuthEntry(NonExistingUserName, "12345"));

    ASSERT_TRUE(fs::exists(NonExistingUserName));
    ASSERT_TRUE(test::CheckFileContent(NonExistingUserName, "12345"));
}
