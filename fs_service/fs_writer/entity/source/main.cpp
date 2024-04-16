#include "entity.hpp"

#include <kos/trace.h>

int main()
{
    const auto rc{nas::FsWriterEntity::Get().Run()};
    if (rc != rcOk)
    {
        ERROR(FS_WRITER_ENTITY, "Entity halted");
        return 1;
    }

    return 0;
}
