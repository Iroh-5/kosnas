#include "fs_service_proxy.hpp"
#include "utils.hpp"

#include <coresrv/sl/sl_api.h>

#include <nas/CommonTypes.idl.h>

#include <kos/trace.h>

#include <algorithm>
#include <stdexcept>
#include <string>

namespace nas {

FsServiceProxy& FsServiceProxy::Get()
{
    static FsServiceProxy instance;

    return instance;
}

Retcode FsServiceProxy::Init(const std::string& username) noexcept
{
    std::string usernameHash;
    if (const auto rc{ProxyBase::Get().GetHash(username, usernameHash)}; rc != rcOk)
    {
        ERROR(AUTH_SERVICE_PROXY, "Failed to get username hash: %d", rc);
        return rc;
    }

    if (const auto rc{CallInitWriter(usernameHash + "/")}; rc != rcOk)
    {
        ERROR(AUTH_SERVICE_PROXY, "Failed to init FsWriter");
        return rc;
    }

    if (const auto rc{CallInitReader(usernameHash + "/")}; rc != rcOk)
    {
        ERROR(AUTH_SERVICE_PROXY, "Failed to init FsReader");
        return rc;
    }

    return rcOk;
}

Retcode FsServiceProxy::Deinit() noexcept
{
    return rcUnimplemented;
}

Retcode FsServiceProxy::SendFile(const std::string& filePath, const std::string& fileContent) noexcept
{
    if (const auto rc{CallInitiateFileReceit(filePath, static_cast<std::uint32_t>(fileContent.size()))}; rc != rcOk)
    {
        return rc;
    }

    for (auto it{fileContent.begin()}; it != fileContent.end();)
    {
        const auto batchSize{
            std::min(
                static_cast<std::uint32_t>(std::distance(it, fileContent.end())),
                nas_CommonTypes_DataBufferSize)};

        if (const auto rc{CallReceiveFileData(std::string{it, std::next(it, batchSize)})}; rc != rcOk)
        {
            return rc;
        }

        std::advance(it, batchSize);
    }

    return rcOk;
}

Retcode FsServiceProxy::GetFile(const std::string& filePath, std::string& fileContent) noexcept
{
    std::uint32_t fileSize{0};
    if (const auto rc{CallInitiateFileTransmission(filePath, fileSize)}; rc != rcOk)
    {
        return rc;
    }

    fileContent.reserve(fileSize);

    while (fileSize > 0)
    {
        const auto batchSize{std::min(fileSize, nas_CommonTypes_DataBufferSize)};

        std::string fileData;
        if (const auto rc{CallTransmitFileData(fileData, batchSize)}; rc != rcOk)
        {
            return rc;
        }

        fileContent += fileData;
        fileSize    -= static_cast<std::uint32_t>(fileData.size());
    }

    return rcOk;
}

FsServiceProxy::FsServiceProxy()
{
    m_readerHandle = ServiceLocatorConnect("FsReaderConnection");
    if (m_readerHandle == INVALID_HANDLE)
    {
        throw std::runtime_error{"ServiceLocatorConnect failed for FsReaderConnection"};
    }

    NkKosTransport_Init(&m_readerTransport, m_readerHandle, nullptr, 0);

    const auto readerRiid{ServiceLocatorGetRiid(m_readerHandle, "nas.FsReader.FsReader")};
    if (readerRiid == INVALID_RIID)
    {
        throw std::runtime_error{"ServiceLocatorGetRiid failed for FsReaderConnection"};
    }

    nas_FsReader_proxy_init(&m_readerProxy, &m_readerTransport.base, readerRiid);

    DEBUG(FS_SERVICE_PROXY, "Initialized proxy for FsReader");

    m_writerHandle = ServiceLocatorConnect("FsWriterConnection");
    if (m_writerHandle == INVALID_HANDLE)
    {
        throw std::runtime_error{"ServiceLocatorConnect failed for FsWriterConnection"};
    }

    NkKosTransport_Init(&m_writerTransport, m_writerHandle, nullptr, 0);

    const auto writerRiid{ServiceLocatorGetRiid(m_writerHandle, "nas.FsWriter.FsWriter")};
    if (writerRiid == INVALID_RIID)
    {
        throw std::runtime_error{"ServiceLocatorGetRiid failed for FsWriterConnection"};
    }

    nas_FsWriter_proxy_init(&m_writerProxy, &m_writerTransport.base, writerRiid);

    DEBUG(FS_SERVICE_PROXY, "Initialized proxy for FsWriter");
}

Retcode FsServiceProxy::CallInitWriter(const std::string& dirPath) noexcept
{
    nas_FsWriter_Init_req req;
    nas_FsWriter_Init_res res;

    nk_req_reset(&req);

    char reqBuffer[nas_FsWriter_Init_req_arena_size];
    nk_arena reqArena = NK_ARENA_INITIALIZER(reqBuffer, reqBuffer + sizeof(reqBuffer));

    nk_arena_reset(&reqArena);

    if (utils::StringToArena(dirPath, &reqArena, &req.dirPath) != rcOk)
    {
        ERROR(FS_SERVICE_PROXY, "Writing to arena failed");
        return rcFail;
    }

    if (const auto rc{nas_FsWriter_Init(&m_writerProxy.base, &req, &reqArena, &res, nullptr)}; rc != NK_EOK)
    {
        ERROR(FS_SERVICE_PROXY, "Init proxy call failed: %d", rc);
        return rcFail;
    }

    return res.rc;
}

Retcode FsServiceProxy::CallInitReader(const std::string& dirPath) noexcept
{
    nas_FsReader_Init_req req;
    nas_FsReader_Init_res res;

    nk_req_reset(&req);

    char reqBuffer[nas_FsReader_Init_req_arena_size];
    nk_arena reqArena = NK_ARENA_INITIALIZER(reqBuffer, reqBuffer + sizeof(reqBuffer));

    nk_arena_reset(&reqArena);

    if (utils::StringToArena(dirPath, &reqArena, &req.filePathString) != rcOk)
    {
        ERROR(FS_SERVICE_PROXY, "Writing to arena failed");
        return rcFail;
    }

    if (const auto rc{nas_FsReader_Init(&m_readerProxy.base, &req, &reqArena, &res, nullptr)}; rc != NK_EOK)
    {
        ERROR(FS_SERVICE_PROXY, "Init proxy call failed: %d", rc);
        return rcFail;
    }

    return res.rc;
}

Retcode FsServiceProxy::CallInitiateFileReceit(const std::string& filePath, std::uint32_t fileSize) noexcept
{
    nas_FsWriter_InitiateFileReceit_req req;
    nas_FsWriter_InitiateFileReceit_res res;

    nk_req_reset(&req);

    char reqBuffer[nas_FsWriter_InitiateFileReceit_req_arena_size];
    nk_arena reqArena = NK_ARENA_INITIALIZER(reqBuffer, reqBuffer + sizeof(reqBuffer));

    nk_arena_reset(&reqArena);

    req.fileSize = fileSize;

    if (utils::StringToArena(filePath, &reqArena, &req.filePathString) != rcOk)
    {
        ERROR(FS_SERVICE_PROXY, "Writing to arena failed");
        return rcFail;
    }

    if (const auto rc{nas_FsWriter_InitiateFileReceit(&m_writerProxy.base, &req, &reqArena, &res, nullptr)}; rc != NK_EOK)
    {
        ERROR(FS_SERVICE_PROXY, "InitiateFileReceit proxy call failed: %d", rc);
        return rcFail;
    }

    return res.rc;
}

Retcode FsServiceProxy::CallReceiveFileData(const std::string& data) noexcept
{
    nas_FsWriter_ReceiveFileData_req req;
    nas_FsWriter_ReceiveFileData_res res;

    nk_req_reset(&req);

    char reqBuffer[nas_FsWriter_ReceiveFileData_req_arena_size];
    nk_arena reqArena = NK_ARENA_INITIALIZER(reqBuffer, reqBuffer + sizeof(reqBuffer));

    nk_arena_reset(&reqArena);

    if (utils::StringToArena(data, &reqArena, &req.dataBuffer) != rcOk)
    {
        ERROR(FS_SERVICE_PROXY, "Writing to arena failed");
        return rcFail;
    }

    if (const auto rc{nas_FsWriter_ReceiveFileData(&m_writerProxy.base, &req, &reqArena, &res, nullptr)}; rc != NK_EOK)
    {
        ERROR(FS_SERVICE_PROXY, "ReceiveFileData proxy call failed: %d", rc);
        return rcFail;
    }

    return res.rc;
}

Retcode FsServiceProxy::CallInitiateFileTransmission(const std::string& filePath, std::uint32_t& fileSize) noexcept
{
    nas_FsReader_InitiateFileTransmission_req req;
    nas_FsReader_InitiateFileTransmission_res res;

    nk_req_reset(&req);

    char reqBuffer[nas_FsReader_InitiateFileTransmission_req_arena_size];
    nk_arena reqArena = NK_ARENA_INITIALIZER(reqBuffer, reqBuffer + sizeof(reqBuffer));

    nk_arena_reset(&reqArena);

    if (utils::StringToArena(filePath, &reqArena, &req.filePathString) != rcOk)
    {
        ERROR(FS_SERVICE_PROXY, "Writing to arena failed");
        return rcFail;
    }

    if (const auto rc{nas_FsReader_InitiateFileTransmission(&m_readerProxy.base, &req, &reqArena, &res, nullptr)}; rc != NK_EOK)
    {
        ERROR(FS_SERVICE_PROXY, "InitiateFileTransmission proxy call failed: %d", rc);
        return rcFail;
    }

    fileSize = res.fileSize;

    return res.rc;
}

Retcode FsServiceProxy::CallTransmitFileData(std::string& fileData, std::uint32_t dataSize) noexcept
{
    nas_FsReader_TransmitFileData_req req;
    nas_FsReader_TransmitFileData_res res;

    nk_req_reset(&req);

    char resBuffer[nas_FsReader_TransmitFileData_res_arena_size];
    nk_arena resArena = NK_ARENA_INITIALIZER(resBuffer, resBuffer + sizeof(resBuffer));

    nk_arena_reset(&resArena);

    req.dataSize = dataSize;

    if (const auto rc{nas_FsReader_TransmitFileData(&m_readerProxy.base, &req, nullptr, &res, &resArena)}; rc != NK_EOK)
    {
        ERROR(FS_SERVICE_PROXY, "TransmitFileData proxy call failed: %d", rc);
        return rcFail;
    }

    fileData = utils::GetArenaString(&resArena, &res.dataBuffer);
    if (fileData.empty())
    {
        ERROR(FS_SERVICE_PROXY, "Failed to get string from arena");
        return rcFail;
    }

    return res.rc;
}

} // namespace nas
