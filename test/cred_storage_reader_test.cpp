#include "cred_storage_reader/lib/include/cred_storage_reader.hpp"

#include <gtest/gtest.h>

namespace {

const std::string NonExistingUser{"NonExistingUser"};
const std::string UserName{"creds/SomeUser"};

const std::string WrongPassHash{"hashpass12345"};
const std::string PassHash{"passhash12345"};

} // namespace

TEST(Authenticate, UnknownUserName_Failure)
{
    auto& credStorageReader{nas::CredStorageReader::Get()};

    ASSERT_EQ(rcResourceNotFound, credStorageReader.Authenticate(NonExistingUser, ""));
}

TEST(Authenticate, WrongPassHash_Failure)
{
    auto& credStorageReader{nas::CredStorageReader::Get()};

    ASSERT_EQ(rcFail, credStorageReader.Authenticate(UserName, WrongPassHash));
}

TEST(Authenticate, GoodScenario)
{
    auto& credStorageReader{nas::CredStorageReader::Get()};

    ASSERT_EQ(rcOk, credStorageReader.Authenticate(UserName, PassHash));
}
