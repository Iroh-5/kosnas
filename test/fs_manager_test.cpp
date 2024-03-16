#include "fs_manager.hpp"

#include <gtest/gtest.h>

#include <rtl/assert.h>

#include <sstream>

namespace fs = std::filesystem;

namespace {

std::string GetTmpDirName()
{
    static auto id{0};

    return "/tmp" + std::to_string(id++);
}

std::string GetTmpFileName()
{
    static auto id{0};

    return "file" + std::to_string(id++);
}

bool CheckFileContent(const fs::path& path, const std::string& fileContentToCmp)
{
    std::ifstream ifs{path};

    rtl_assert(ifs.is_open());

    std::stringstream fileContent;
    fileContent << ifs.rdbuf();

    return fileContent.str() == fileContentToCmp;
}

} // namespace

TEST(Basic, Init_EmptyPath)
{
    auto& fsManager{nas::FsManager::Get()};

    ASSERT_EQ(rcFail, fsManager.Init(fs::path{}));
    ASSERT_EQ(rcOk,   fsManager.Deinit());
}

TEST(Basic, Init_GoodPath)
{
    auto& fsManager{nas::FsManager::Get()};

    static const fs::path Path{GetTmpDirName()};

    ASSERT_EQ(rcOk, fsManager.Init(Path));
    ASSERT_EQ(rcOk, fsManager.Deinit());
}

TEST(Basic, Init_BadPath)
{
    auto& fsManager{nas::FsManager::Get()};

    static const fs::path Path{"/"};

    ASSERT_EQ(rcFail, fsManager.Init(Path));
    ASSERT_EQ(rcOk,   fsManager.Deinit());
}

TEST(FileReceit, InitFileReceit_BadFileSize)
{
    auto& fsManager{nas::FsManager::Get()};

    ASSERT_EQ(rcOk,          fsManager.Init(GetTmpDirName(), 1u));
    ASSERT_EQ(rcOutOfMemory, fsManager.InitiateFileReceit(GetTmpFileName(), 2u));
    ASSERT_EQ(rcOk,          fsManager.Deinit());
}

TEST(FileReceit, InitFileReceit_BadFilePath)
{
    auto& fsManager{nas::FsManager::Get()};

    ASSERT_EQ(rcOk,   fsManager.Init(GetTmpDirName(), 1u));
    ASSERT_EQ(rcFail, fsManager.InitiateFileReceit(fs::path{"/non-existing-path/blah"}, 1u));
    ASSERT_EQ(rcOk,   fsManager.Deinit());
}

TEST(FileReceit, InitFileReceit_Good)
{
    auto& fsManager{nas::FsManager::Get()};

    const auto DirName {GetTmpDirName()};
    const auto FileName{GetTmpFileName()};

    ASSERT_EQ(rcOk, fsManager.Init(DirName, 2u));
    ASSERT_EQ(rcOk, fsManager.InitiateFileReceit(FileName, 1));
    ASSERT_EQ(rcOk, fsManager.Deinit());

    ASSERT_TRUE(fs::exists(fs::path{DirName} / fs::path{FileName}));
}

TEST(FileReceit, ReceiveFileData_BadState)
{
    ASSERT_EQ(rcInvalidOperation, nas::FsManager::Get().ReceiveFileData({}));
}

TEST(FileReceit, ReceiveFileData_OrdinarCase)
{
    auto& fsManager{nas::FsManager::Get()};

    const auto DirName{GetTmpDirName()};
    const auto FileName{GetTmpFileName()};

    const std::string Msg{"Hello, world!"};

    ASSERT_EQ(rcOk, fsManager.Init(DirName));
    ASSERT_EQ(rcOk, fsManager.InitiateFileReceit(FileName, Msg.size()));

    for (char c : Msg)
    {
        ASSERT_EQ(rcOk, fsManager.ReceiveFileData({c}));
    }

    ASSERT_EQ(rcOk, fsManager.Deinit());

    const auto FilePath{fs::path{DirName} / fs::path{FileName}};
    ASSERT_TRUE(fs::exists(FilePath));
    ASSERT_TRUE(CheckFileContent(FilePath, Msg));
}

TEST(FileReceit, ReceiveFileData_PassMoreThanPromissed)
{
    auto& fsManager{nas::FsManager::Get()};

    const auto DirName{GetTmpDirName()};
    const auto FileName{GetTmpFileName()};

    const std::string Msg{"Hello, world!"};

    ASSERT_EQ(rcOk, fsManager.Init(DirName));
    ASSERT_EQ(rcOk, fsManager.InitiateFileReceit(FileName, Msg.size() / 2));

    for (std::size_t i{0}; i < Msg.size() / 2; ++i)
    {
        ASSERT_EQ(rcOk, fsManager.ReceiveFileData({ Msg[i] }));
    }
    ASSERT_EQ(rcInvalidOperation, fsManager.ReceiveFileData({ Msg[Msg.size() / 2] }));

    ASSERT_EQ(rcOk, fsManager.Deinit());

    const auto FilePath{fs::path{DirName} / fs::path{FileName}};
    ASSERT_TRUE(fs::exists(FilePath));
    ASSERT_TRUE(CheckFileContent(FilePath, Msg.substr(0, Msg.size() / 2)));
}

TEST(FileTransmission, InitFileTransmission_BadFilePath)
{
    auto& fsManager{nas::FsManager::Get()};

    std::size_t size{0};
    ASSERT_EQ(rcOk,   fsManager.Init(GetTmpDirName()));
    ASSERT_EQ(rcFail, fsManager.InitiateFileReceit(fs::path{"/non-existing-path/blah"}, size));
    ASSERT_EQ(rcOk,   fsManager.Deinit());
}

TEST(FileTransmission, InitFileTransmission_Good)
{
    auto& fsManager{nas::FsManager::Get()};

    const auto DirName {GetTmpDirName()};
    const auto FileName{GetTmpFileName()};

    const std::string Msg{"Hello, world!"};

    ASSERT_EQ(rcOk, fsManager.Init(DirName));
    ASSERT_EQ(rcOk, fsManager.InitiateFileReceit(FileName, Msg.size()));

    ASSERT_EQ(rcOk, fsManager.ReceiveFileData(std::vector<std::uint8_t>{Msg.cbegin(), Msg.cend()}));

    std::size_t fileSize{0};
    ASSERT_EQ(rcOk, fsManager.InitiateFileTransmission(FileName, fileSize));
    ASSERT_EQ(Msg.size(), fileSize);

    ASSERT_EQ(rcOk, fsManager.Deinit());

    ASSERT_TRUE(fs::exists(fs::path{DirName} / fs::path{FileName}));
}

TEST(FileTransmission, TransmitFileData_BadState)
{
    std::vector<std::uint8_t> receivedData;
    ASSERT_EQ(rcInvalidOperation, nas::FsManager::Get().TransmitFileData(receivedData, 0u));
}

TEST(FileTransmission, TransmitFileData_BadArgs)
{
    auto& fsManager{nas::FsManager::Get()};

    const auto DirName {GetTmpDirName()};
    const auto FileName{GetTmpFileName()};

    const std::string Msg{"Hello, world!"};

    ASSERT_EQ(rcOk, fsManager.Init(DirName));
    ASSERT_EQ(rcOk, fsManager.InitiateFileReceit(FileName, Msg.size()));

    ASSERT_EQ(rcOk, fsManager.ReceiveFileData(std::vector<std::uint8_t>{Msg.cbegin(), Msg.cend()}));

    std::size_t fileSize{0};
    ASSERT_EQ(rcOk, fsManager.InitiateFileTransmission(FileName, fileSize));
    ASSERT_EQ(Msg.size(), fileSize);

    std::vector<std::uint8_t> receivedData;
    receivedData.resize(Msg.size() - 1);

    ASSERT_EQ(rcInvalidArgument, nas::FsManager::Get().TransmitFileData(receivedData, Msg.size()));

    ASSERT_EQ(rcOk, fsManager.Deinit());
}

TEST(FileTransmission, TransmitFileData_OrdinarCase)
{
    auto& fsManager{nas::FsManager::Get()};

    const auto DirName {GetTmpDirName()};
    const auto FileName{GetTmpFileName()};

    const std::string Msg{"Hello, world!"};

    ASSERT_EQ(rcOk, fsManager.Init(DirName));
    ASSERT_EQ(rcOk, fsManager.InitiateFileReceit(FileName, Msg.size()));

    ASSERT_EQ(rcOk, fsManager.ReceiveFileData(std::vector<std::uint8_t>{Msg.cbegin(), Msg.cend()}));

    std::size_t fileSize{0};
    ASSERT_EQ(rcOk, fsManager.InitiateFileTransmission(FileName, fileSize));
    ASSERT_EQ(Msg.size(), fileSize);

    std::string receivedMsg;
    for (std::size_t i{0}; i < Msg.size(); ++i)
    {
        std::vector<std::uint8_t> receivedData{1};

        ASSERT_EQ(rcOk, fsManager.TransmitFileData(receivedData, 1u));

        receivedMsg += std::string{receivedData.cbegin(), receivedData.cend()};
    }

    ASSERT_EQ(Msg, receivedMsg);

    ASSERT_EQ(rcOk, fsManager.Deinit());
}
