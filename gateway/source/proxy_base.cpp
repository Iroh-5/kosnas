#include "proxy_base.hpp"
#include "utils.hpp"

#include <coresrv/sl/sl_api.h>

#include <kos/trace.h>

#include <stdexcept>
#include <string>

namespace nas {

ProxyBase& ProxyBase::Get()
{
    static ProxyBase instance;

    return instance;
}

Retcode ProxyBase::GetHash(const std::string& str, std::string& hash) noexcept
{
    nas_CryptoModule_GetSHA256Hash_req req;
    nas_CryptoModule_GetSHA256Hash_res res;

    nk_req_reset(&req);

    char reqBuffer[nas_CryptoModule_GetSHA256Hash_req_arena_size];
    nk_arena reqArena = NK_ARENA_INITIALIZER(reqBuffer, reqBuffer + sizeof(reqBuffer));

    nk_arena_reset(&reqArena);

    if (utils::StringToArena(str, &reqArena, &req.str) != rcOk)
    {
        ERROR(PROXY_BASE, "Writing to arena failed");
        return rcFail;
    }

    char resBuffer[nas_CryptoModule_GetSHA256Hash_res_arena_size];
    nk_arena resArena = NK_ARENA_INITIALIZER(resBuffer, resBuffer + sizeof(resBuffer));

    nk_arena_reset(&resArena);

    if (const auto rc{nas_CryptoModule_GetSHA256Hash(&m_cryptoProxy.base, &req, &reqArena, &res, &resArena)}; rc != NK_EOK)
    {
        ERROR(PROXY_BASE, "GetSHA256Hash proxy call failed: %d", rc);
        return rcFail;
    }

    hash = utils::GetArenaString(&resArena, &res.hash);

    return res.rc;
}

ProxyBase::ProxyBase()
{
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

    DEBUG(PROXY_BASE, "Initialized proxy for CryptoModule");
}

} // namespace nas
