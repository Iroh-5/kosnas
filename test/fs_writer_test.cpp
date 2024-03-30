#include "fs_test_common.hpp"
#include "fs_writer/lib/include/fs_writer.hpp"

#include <gtest/gtest.h>

#include <rtl/assert.h>

namespace fs = std::filesystem;

TEST(Basic, Init_DirectoryCreated)
{
    auto& fsWriter{nas::FsWriter::Get()};

    const auto dirPathStr{test::GetTmpDirPathStr()};

    ASSERT_EQ(rcOk, fsWriter.Init(dirPathStr));
    ASSERT_EQ(rcOk, fsWriter.Deinit());

    ASSERT_TRUE(fs::exists(dirPathStr));
}

TEST(FileReceit, InitFileReceit_BadFileSize)
{
    auto& fsWriter{nas::FsWriter::Get()};

    ASSERT_EQ(rcOk,          fsWriter.Init(test::GetTmpDirPathStr(), 1u));
    ASSERT_EQ(rcOutOfMemory, fsWriter.InitiateFileReceit(test::GetTmpFilePathStr(), 2u));
    ASSERT_EQ(rcOk,          fsWriter.Deinit());
}

TEST(FileReceit, InitFileReceit_BadFilePath)
{
    auto& fsWriter{nas::FsWriter::Get()};

    ASSERT_EQ(rcOk,   fsWriter.Init(test::GetTmpDirPathStr(), 1u));
    ASSERT_EQ(rcFail, fsWriter.InitiateFileReceit(fs::path{"/"}, 1u));
    ASSERT_EQ(rcOk,   fsWriter.Deinit());
}

TEST(FileReceit, InitFileReceit_Good)
{
    auto& fsWriter{nas::FsWriter::Get()};

    const auto dirPathStr {test::GetTmpDirPathStr()};
    const auto filePathStr{test::GetTmpFilePathStr()};

    ASSERT_EQ(rcOk, fsWriter.Init(dirPathStr, 2u));
    ASSERT_EQ(rcOk, fsWriter.InitiateFileReceit(filePathStr, 1));
    ASSERT_EQ(rcOk, fsWriter.Deinit());

    ASSERT_TRUE(fs::exists(test::GetFullFilePathStr(dirPathStr, filePathStr)));
}

TEST(FileReceit, ReceiveFileData_BadState)
{
    ASSERT_EQ(rcInvalidOperation, nas::FsWriter::Get().ReceiveFileData({}));
}

TEST(FileReceit, ReceiveFileData_OrdinarCase)
{
    auto& fsWriter{nas::FsWriter::Get()};

    const auto dirPathStr {test::GetTmpDirPathStr()};
    const auto filePathStr{test::GetTmpFilePathStr()};

    const std::string msg{"Hello, world!"};

    ASSERT_EQ(rcOk, fsWriter.Init(dirPathStr));
    ASSERT_EQ(rcOk, fsWriter.InitiateFileReceit(filePathStr, msg.size()));

    for (char c : msg)
    {
        ASSERT_EQ(rcOk, fsWriter.ReceiveFileData({c}));
    }

    ASSERT_EQ(rcOk, fsWriter.Deinit());

    const auto fileFullPathStr{test::GetFullFilePathStr(dirPathStr, filePathStr)};
    ASSERT_TRUE(fs::exists(fileFullPathStr));
    ASSERT_TRUE(test::CheckFileContent(fileFullPathStr, msg));
}

TEST(FileReceit, ReceiveFileData_PassMoreThanPromissed)
{
    auto& fsWriter{nas::FsWriter::Get()};

    const auto dirPathStr {test::GetTmpDirPathStr()};
    const auto filePathStr{test::GetTmpFilePathStr()};

    const std::string msg{"Hello, world!"};

    ASSERT_EQ(rcOk, fsWriter.Init(dirPathStr));
    ASSERT_EQ(rcOk, fsWriter.InitiateFileReceit(filePathStr, msg.size() / 2));

    for (std::size_t i{0}; i < msg.size() / 2; ++i)
    {
        ASSERT_EQ(rcOk, fsWriter.ReceiveFileData({ msg[i] }));
    }
    ASSERT_EQ(rcInvalidOperation, fsWriter.ReceiveFileData({ msg[msg.size() / 2] }));

    ASSERT_EQ(rcOk, fsWriter.Deinit());

    const auto fileFullPathStr{test::GetFullFilePathStr(dirPathStr, filePathStr)};
    ASSERT_TRUE(fs::exists(fileFullPathStr));
    ASSERT_TRUE(test::CheckFileContent(fileFullPathStr, msg.substr(0, msg.size() / 2)));
}
