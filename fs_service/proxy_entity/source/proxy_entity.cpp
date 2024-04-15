#include "proxy_entity.hpp"

#include <coresrv/sl/sl_api.h>

#include <kos/trace.h>

int main()
{
    Handle fsReaderHandle = ServiceLocatorConnect("FsReaderConnection");
    if (fsReaderHandle == INVALID_HANDLE)
    {
        ERROR(PROXY_ENTITY, "Could not connect to FsReaderConnection");
        return 1;
    }

    INFO(PROXY_ENTITY, "Successfully connected to FsReaderService");

    Handle fsWriterHandle = ServiceLocatorConnect("FsWriterConnection");
    if (fsWriterHandle == INVALID_HANDLE)
    {
        ERROR(PROXY_ENTITY, "Could not connect to FsWriterConnection");
        return 1;
    }

    INFO(PROXY_ENTITY, "Successfully connected to FsWriterService");

    return 0;
}
