#include "entity.hpp"

#include <kos/trace.h>

int main()
{
    const auto rc{nas::CredStorageReaderEntity::Get().Run()};
    if (rc != rcOk)
    {
        ERROR(CRED_STORAGE_READER_ENTITY, "Entity halted");
        return 1;
    }

    return 0;
}
