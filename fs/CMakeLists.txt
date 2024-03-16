project (fs)

add_subdirectory (lib)

if (NOT CMAKE_BUILD_TESTS)
    add_subdirectory (entity)
endif ()
