#pragma once

#include "proxy_base.hpp"

#include <coresrv/nk/transport-kos.h>

#include <nas/CredStorageReader.idl.h>
#include <nas/CredStorageWriter.idl.h>

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

    Retcode CallAddAuthEntry    (const std::string username,  const std::string& passHash) noexcept;
    Retcode CallAuthenticateUser(const std::string& username, const std::string& passHash) noexcept;

private:
    NkKosTransport              m_readerTransport;
    nas_CredStorageReader_proxy m_readerProxy;
    Handle                      m_readerHandle;

    NkKosTransport              m_writerTransport;
    nas_CredStorageWriter_proxy m_writerProxy;
    Handle                      m_writerHandle;
};

} // namespace nas
