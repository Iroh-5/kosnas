#pragma once

#include "proxy_base.hpp"

#include <coresrv/nk/transport-kos.h>

#include <nas/FsReader.idl.h>
#include <nas/FsWriter.idl.h>

#include <cstdint>
#include <string>

namespace nas {

class FsServiceProxy
{
public:
    static FsServiceProxy& Get();

    Retcode Init(const std::string& username) noexcept;
    Retcode Deinit() noexcept;

    Retcode SendFile(const std::string& filePath, const std::string& fileContent) noexcept;
    Retcode GetFile(const std::string& filePath, std::string& fileContent) noexcept;

private:
    FsServiceProxy();

    Retcode CallInitWriter(const std::string& dirPath) noexcept;
    Retcode CallInitReader(const std::string& dirPath) noexcept;

    Retcode CallInitiateFileReceit(const std::string& filePath, std::uint32_t fileSize) noexcept;
    Retcode CallReceiveFileData(const std::string& data) noexcept;

    Retcode CallInitiateFileTransmission(const std::string& filePath, std::uint32_t& fileSize) noexcept;
    Retcode CallTransmitFileData(std::string& fileData, std::uint32_t dataSize) noexcept;

private:
    NkKosTransport     m_readerTransport;
    nas_FsReader_proxy m_readerProxy;
    Handle             m_readerHandle;

    NkKosTransport     m_writerTransport;
    nas_FsWriter_proxy m_writerProxy;
    Handle             m_writerHandle;
};

} // namespace nas
