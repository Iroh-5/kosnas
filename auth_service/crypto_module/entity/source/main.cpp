#include "entity.hpp"

#include <kos/trace.h>

int main()
{
    const auto rc{nas::CryptoModuleEntity::Get().Run()};
    if (rc != rcOk)
    {
        ERROR(CRYPTO_MODULE_ENTITY, "Entity halted");
        return 1;
    }

    return 0;
}
