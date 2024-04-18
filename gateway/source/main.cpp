#include "auth_service_proxy.hpp"

#include <kos/trace.h>

int main()
{
    INFO(GATEWAY_ENTITY, "Entity started");

    auto rc = nas::AuthServiceProxy::Get().RegisterUser("1", "2");
    INFO(GATEWAY_ENTITY, "RegisterUser returned rc: %d", rc);

    rc = nas::AuthServiceProxy::Get().AuthenticateUser("1", "2");
    INFO(GATEWAY_ENTITY, "AuthenticateUser user returned rc: %d", rc);

    return 0;
}
