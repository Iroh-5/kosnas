project (FsService)

add_subdirectory (common)
add_subdirectory (fs_reader)
add_subdirectory (fs_writer)

if (NOT CMAKE_BUILD_TESTS)
    add_subdirectory (proxy_entity)
endif ()
