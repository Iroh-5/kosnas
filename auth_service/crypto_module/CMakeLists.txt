project (CryptoModule)

add_subdirectory (PicoSHA2)

if (NOT CMAKE_BUILD_TESTS)
    add_subdirectory (entity)
endif ()
