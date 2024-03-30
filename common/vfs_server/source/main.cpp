#include <kos/trace.h>

#include <fmt/format.h>

#include <string_view>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define _VFS_SERVER_INTERFACE
#include <vfs/basic.h>

namespace {

constexpr std::string_view MountPoint{"/"};
constexpr std::string_view FilesystemType{"ext2"};
constexpr std::string_view Blkdev{"ramdisk0"};

bool MountFileSystem(unsigned partitionId)
{
    const auto fsPath{fmt::format("{}_p{}", Blkdev, partitionId)};

    if (mount(fsPath.c_str(), MountPoint.data(), FilesystemType.data(), 0, ""))
    {
        ERROR(CUSTOM_VFS_SERVER
            "Failed to mount %s, %s, %s. Error %s",
            fsPath.c_str(),
            MountPoint,
            FilesystemType,
            strerror(errno));

        return false;
    }

    return true;
}

} // namespace

static _vfs_server_thread_routine_t threadRoutine{nullptr};
static void *threadRoutineArg{nullptr};

static const char EntityName[] = "VfsUser";

void _vfs_server_set_main_thread_routine(_vfs_server_thread_routine_t func,
                                         void *arg)
{
    threadRoutine = func;
    threadRoutineArg = arg;
}

int main()
{
    if (MountFileSystem(PARTITION))
    {
        return EXIT_FAILURE;
    }

    if (threadRoutine == nullptr)
    {
        ERROR(CUSTOM_VFS_SERVER, "threadRoutine pointer is null! Terminating!");

        return EXIT_FAILURE;
    }

    (*threadRoutine)(threadRoutineArg);

    return EXIT_SUCCESS;
}
