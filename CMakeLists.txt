cmake_minimum_required (VERSION 3.12)

project (nas)

include (platform)
initialize_platform (FORCE_STATIC)

include (platform/nk)

find_package (vfs REQUIRED)
include_directories (${vfs_INCLUDE})

find_package (blkdev REQUIRED)
find_package (LWEXT4 REQUIRED)
find_package (fmt REQUIRED)
find_package (precompiled_vfs REQUIRED)
find_package (usb REQUIRED)
find_package (sdcard REQUIRED)

set (VULN_LDFLAGS "\
    -Wl,-z,noexecstack \
    -Wl,-z,separate-code \
    -Wl,-z,now")

set (VULN_COMPILER_FLAGS "\
    -Werror -Wall -Wextra -Wconversion \
    -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 \
    -fstack-protector-strong -fstack-clash-protection \
    -mbranch-protection=standard \
    -Wsign-conversion -Wformat=2 -Wformat-security -Werror=format-security \
    -fsanitize=undefined -fsanitize-undefined-trap-on-error")

set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${VULN_COMPILER_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VULN_COMPILER_FLAGS}")
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/common/")

nk_build_idl_files (
    common_types_idl
    NK_MODULE "nas"
    IDL "${CMAKE_SOURCE_DIR}/res/CommonTypes.idl")

add_subdirectory (fs_service)

if (CMAKE_BUILD_TESTS)
    add_subdirectory (test)
endif ()

add_subdirectory (einit)
