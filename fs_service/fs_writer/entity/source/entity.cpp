#include "entity.hpp"

#include <coresrv/sl/sl_api.h>

#include <kos/trace.h>

int main()
{
    ServiceId iid;
    Handle handle = ServiceLocatorRegister("FsWriterConnection", nullptr, 0, &iid);

    if (handle == INVALID_HANDLE)
    {
        ERROR(FS_WRITER_ENTITY, "Could not register service");
        return 1;
    }

    INFO(FS_WRITER_ENTITY, "Service registered successfully");

    return 0;
}
