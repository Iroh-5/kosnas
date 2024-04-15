project (einit)

include (platform/image)

project_header_default ("STANDARD_GNU_11:YES" "STRICT_WARNINGS:NO")

if (CMAKE_BUILD_TESTS)
    set (SECURITY_PSL_FILE "src/test_security.psl.in")
    set (INIT_FILE "src/test_init.yaml.in")
    set (ENTITIES TestEntity)
else ()
    set (SECURITY_PSL_FILE "src/security.psl.in")
    set (INIT_FILE "src/init.yaml.in")
    set (ENTITIES
            FsReaderEntity
            FsWriterEntity
            FsProxyEntity
            FsReaderVfs
            FsWriterVfs)
endif ()

set (FS_IMG_SIZE 64)
set (FS_IMG_FORMAT fat32)
set (FS_IMAGE ramdisk0.img)
set (FILES ${CMAKE_BINARY_DIR}/einit/${FS_IMAGE})

add_custom_target (${FS_IMAGE}
    COMMAND  ${KL_SDK_ROOT_PATH}/common/prepare_hdd_img.sh
        -%1  50
        -%2  50
        -f1  "ext2"
        -f2  "ext2"
        -p1  ${CMAKE_SOURCE_DIR}/res/hdd/p1
        -p2  ${CMAKE_SOURCE_DIR}/res/hdd/p2
        -s   ${FS_IMG_SIZE}
        -img ${FS_IMAGE})

build_kos_hw_image (kos-image
                    EINIT_ENTITY EinitHw
                    CONNECTIONS_CFG ${INIT_FILE}
                    SECURITY_PSL ${SECURITY_PSL_FILE}
                    IMAGE_FILES ${ENTITIES} ${FILES})

add_dependencies (kos-image ${FS_IMAGE})

set_target_properties (
    EinitHw
    PROPERTIES
        LINK_FLAGS ${VULN_LDFLAGS})
