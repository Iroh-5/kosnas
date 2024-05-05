#pragma once

#include <coresrv/nk/transport-kos.h>

#include <nas/CryptoModule.idl.h>

#include <string>

namespace nas {

class ProxyBase
{
public:
    static ProxyBase& Get();

    Retcode GetHash(const std::string& str, std::string& hash) noexcept;

private:
    ProxyBase();

private:
    NkKosTransport         m_cryptoTransport;
    nas_CryptoModule_proxy m_cryptoProxy;
    Handle                 m_cryptoHandle;
};

} // namespace nas
