project (CredStorageWriter)

add_subdirectory (lib)

if (NOT CMAKE_BUILD_TESTS)
    include (vfs_server/custom_vfs_server)

    set (VFS_SERVER_TARGET CredStorageVfs)
    set (VFS_SERVER_PART 1)

    create_custom_vfs_server (${VFS_SERVER_TARGET} ${VFS_SERVER_PART})

    add_subdirectory (entity)
endif ()
