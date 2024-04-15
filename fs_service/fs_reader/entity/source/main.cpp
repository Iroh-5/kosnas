#include "entity.hpp"

#include <kos/trace.h>

int main()
{
    const auto rc{nas::FsReaderEntity::Get().Run()};
    if (rc != rcOk)
    {
        ERROR(FS_READER_ENTITY, "Entity halted");
    }

    return 0;
}
