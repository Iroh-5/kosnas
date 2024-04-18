#include "auth_service_proxy.hpp"
#include "utils.hpp"

#include <coresrv/sl/sl_api.h>

#include <kos/trace.h>

#include <stdexcept>

namespace nas {

AuthServiceProxy& AuthServiceProxy::Get()
{
    static AuthServiceProxy instance;

    return instance;
}

Retcode AuthServiceProxy::RegisterUser(const std::string& name, const std::string& password) noexcept
{
    nas_CredStorageWriter_AddAuthEntry_req req;
    nas_CredStorageWriter_AddAuthEntry_res res;

    nk_req_reset(&req);

    char reqBuffer[nas_CredStorageWriter_AddAuthEntry_req_arena_size];
    nk_arena reqArena = NK_ARENA_INITIALIZER(reqBuffer, reqBuffer + sizeof(reqBuffer));

    nk_arena_reset(&reqArena);

    if (utils::StringToArena(name, &reqArena, &req.username) != rcOk)
    {
        ERROR(AUTH_SERVICE_PROXY, "Writing to arena failed");
        return rcFail;
    }

    if (utils::StringToArena(password, &reqArena, &req.passwordHash) != rcOk)
    {
        ERROR(AUTH_SERVICE_PROXY, "Writing to arena failed");
        return rcFail;
    }

    if (const auto rc{nas_CredStorageWriter_AddAuthEntry(&m_writerProxy.base, &req, &reqArena, &res, nullptr)}; rc != NK_EOK)
    {
        ERROR(AUTH_SERVICE_PROXY, "AddAuthEntry proxy call failed: %d", rc);
        return rcFail;
    }

    return res.rc;
}

Retcode AuthServiceProxy::AuthenticateUser(const std::string& name, const std::string& password) noexcept
{
    nas_CredStorageReader_Authenticate_req req;
    nas_CredStorageReader_Authenticate_res res;

    nk_req_reset(&req);

    char reqBuffer[nas_CredStorageReader_Authenticate_req_arena_size];
    nk_arena reqArena = NK_ARENA_INITIALIZER(reqBuffer, reqBuffer + sizeof(reqBuffer));

    nk_arena_reset(&reqArena);

    if (utils::StringToArena(name, &reqArena, &req.username) != rcOk)
    {
        ERROR(AUTH_SERVICE_PROXY, "Writing to arena failed");
        return rcFail;
    }

    if (utils::StringToArena(password, &reqArena, &req.passwordHash) != rcOk)
    {
        ERROR(AUTH_SERVICE_PROXY, "Writing to arena failed");
        return rcFail;
    }

    if (const auto rc{nas_CredStorageReader_Authenticate(&m_readerProxy.base, &req, &reqArena, &res, nullptr)}; rc != NK_EOK)
    {
        ERROR(AUTH_SERVICE_PROXY, "Authenticate proxy call failed: %d", rc);
        return rcFail;
    }

    return res.rc;
}

Retcode AuthServiceProxy::DeauthenticateUser() noexcept
{
    return rcUnimplemented;
}

AuthServiceProxy::AuthServiceProxy()
{
    m_readerHandle = ServiceLocatorConnect("CredStorageReaderConnection");
    if (m_readerHandle == INVALID_HANDLE)
    {
        throw std::runtime_error{"ServiceLocatorConnect failed for CredStorageReaderConnection"};
    }

    NkKosTransport_Init(&m_readerTransport, m_readerHandle, nullptr, 0);

    const auto readerRiid{ServiceLocatorGetRiid(m_readerHandle, "nas.CredStorageReader.CredStorageReader")};
    if (readerRiid == INVALID_RIID)
    {
        throw std::runtime_error{"ServiceLocatorGetRiid failed for CredStorageReaderConnection"};
    }

    nas_CredStorageReader_proxy_init(&m_readerProxy, &m_readerTransport.base, readerRiid);

    DEBUG(AUTH_SERVICE_PROXY, "Initialized proxy for CredStorageReader");

    m_writerHandle = ServiceLocatorConnect("CredStorageWriterConnection");
    if (m_writerHandle == INVALID_HANDLE)
    {
        throw std::runtime_error{"ServiceLocatorConnect failed for CredStorageWriterConnection"};
    }

    NkKosTransport_Init(&m_writerTransport, m_writerHandle, nullptr, 0);

    const auto writerRiid{ServiceLocatorGetRiid(m_writerHandle, "nas.CredStorageWriter.CredStorageWriter")};
    if (writerRiid == INVALID_RIID)
    {
        throw std::runtime_error{"ServiceLocatorGetRiid failed for CredStorageWriterConnection"};
    }

    nas_CredStorageWriter_proxy_init(&m_writerProxy, &m_writerTransport.base, writerRiid);

    DEBUG(AUTH_SERVICE_PROXY, "Initialized proxy for CredStorageWriter");

    m_cryptoHandle = ServiceLocatorConnect("CryptoModuleConnection");
    if (m_cryptoHandle == INVALID_HANDLE)
    {
        throw std::runtime_error{"ServiceLocatorConnect failed for CryptoModuleConnection"};
    }

    NkKosTransport_Init(&m_cryptoTransport, m_cryptoHandle, nullptr, 0);

    const auto cryptoRiid{ServiceLocatorGetRiid(m_cryptoHandle, "nas.CryptoModule.CryptoModule")};
    if (cryptoRiid == INVALID_RIID)
    {
        throw std::runtime_error{"ServiceLocatorGetRiid failed for CryptoModuleConnection"};
    }

    nas_CryptoModule_proxy_init(&m_cryptoProxy, &m_cryptoTransport.base, cryptoRiid);

    DEBUG(AUTH_SERVICE_PROXY, "Initialized proxy for CryptoModule");
}

} // namespace nas
