#pragma once

#include <coresrv/nk/transport-kos.h>

#include <nas/CredStorageReader.idl.h>
#include <nas/CredStorageWriter.idl.h>
#include <nas/CryptoModule.idl.h>

#include <string>

namespace nas {

class AuthServiceProxy
{
public:
    static AuthServiceProxy& Get();

    Retcode RegisterUser      (const std::string& name, const std::string& password) noexcept;
    Retcode AuthenticateUser  (const std::string& name, const std::string& password) noexcept;
    Retcode DeauthenticateUser() noexcept;

private:
    AuthServiceProxy();

    Retcode GetPasswordHash(const std::string& password, std::string& passHash) noexcept;

    Retcode CallAddAuthEntry    (const std::string username,  const std::string& passHash) noexcept;
    Retcode CallAuthenticateUser(const std::string& username, const std::string& passHash) noexcept;

private:
    NkKosTransport              m_readerTransport;
    nas_CredStorageReader_proxy m_readerProxy;
    Handle                      m_readerHandle;

    NkKosTransport              m_writerTransport;
    nas_CredStorageWriter_proxy m_writerProxy;
    Handle                      m_writerHandle;

    NkKosTransport              m_cryptoTransport;
    nas_CryptoModule_proxy      m_cryptoProxy;
    Handle                      m_cryptoHandle;
};

} // namespace nas
