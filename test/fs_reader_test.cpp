#include "fs_test_common.hpp"
#include "fs_reader/lib/include/fs_reader.hpp"

#include <gtest/gtest.h>

#include <rtl/assert.h>

namespace fs = std::filesystem;

namespace {

const fs::path NonExistingDirPath {"/non-existing-dir/"};
const fs::path NonExistingFilePath{"non-existing-file"};

const fs::path    DirPath    {"dir/"};
const fs::path    FilePath   {"file"};
const std::string FileContent{"Hello, world!"};

} // namespace

TEST(Basic, Init_BadDirPath)
{
    auto& fsReader{nas::FsReader::Get()};

    ASSERT_EQ(rcFail, fsReader.Init(NonExistingDirPath));
}

TEST(Basic, Init_GoodDirPath)
{
    auto& fsReader{nas::FsReader::Get()};

    ASSERT_EQ(rcOk, fsReader.Init(DirPath));
}

TEST(FileTransmission, InitFileTransmission_BadFilePath)
{
    auto& fsReader{nas::FsReader::Get()};

    ASSERT_EQ(rcOk, fsReader.Init(DirPath));

    std::size_t size{0};
    ASSERT_EQ(rcFail, fsReader.InitiateFileTransmission(NonExistingFilePath, size));
    ASSERT_EQ(rcOk,   fsReader.Deinit());
}

TEST(FileTransmission, InitFileTransmission_Good)
{
    auto& fsReader{nas::FsReader::Get()};

    ASSERT_EQ(rcOk, fsReader.Init(DirPath));

    std::size_t fileSize{0};
    ASSERT_EQ(rcOk, fsReader.InitiateFileTransmission(FilePath, fileSize));
    ASSERT_EQ(FileContent.size(), fileSize);

    ASSERT_EQ(rcOk, fsReader.Deinit());
}

TEST(FileTransmission, TransmitFileData_BadState)
{
    std::vector<std::uint8_t> receivedData;
    ASSERT_EQ(rcInvalidOperation, nas::FsReader::Get().TransmitFileData(receivedData, 0u));
}

TEST(FileTransmission, TransmitFileData_BadArgs)
{
    auto& fsReader{nas::FsReader::Get()};

    ASSERT_EQ(rcOk, fsReader.Init(DirPath));

    std::size_t fileSize{0};
    ASSERT_EQ(rcOk, fsReader.InitiateFileTransmission(FilePath, fileSize));
    ASSERT_EQ(FileContent.size(), fileSize);

    std::vector<std::uint8_t> receivedData;
    receivedData.resize(FileContent.size() - 1);

    ASSERT_EQ(rcInvalidArgument, fsReader.TransmitFileData(receivedData, FileContent.size()));

    ASSERT_EQ(rcOk, fsReader.Deinit());
}

TEST(FileTransmission, TransmitFileData_OrdinarCase)
{
    auto& fsReader{nas::FsReader::Get()};

    ASSERT_EQ(rcOk, fsReader.Init(DirPath));

    std::size_t fileSize{0};
    ASSERT_EQ(rcOk, fsReader.InitiateFileTransmission(FilePath, fileSize));
    ASSERT_EQ(FileContent.size(), fileSize);

    std::string receivedMsg;
    for (std::size_t i{0}; i < FileContent.size(); ++i)
    {
        std::vector<std::uint8_t> receivedData{1};

        ASSERT_EQ(rcOk, fsReader.TransmitFileData(receivedData, 1u));

        receivedMsg += std::string{receivedData.cbegin(), receivedData.cend()};
    }

    ASSERT_EQ(FileContent, receivedMsg);

    ASSERT_EQ(rcOk, fsReader.Deinit());
}
