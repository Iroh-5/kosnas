project (FsWriter)

add_subdirectory (lib)

if (NOT CMAKE_BUILD_TESTS)
    include (vfs_server/custom_vfs_server)

    set (VFS_SERVER_TARGET FsVfs)
    set (VFS_SERVER_PART 0)

    create_custom_vfs_server (${VFS_SERVER_TARGET} ${VFS_SERVER_PART})

    add_subdirectory (entity)
endif ()
