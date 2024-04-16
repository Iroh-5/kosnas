project (einit)

include (platform/image)

project_header_default ("STANDARD_GNU_11:YES" "STRICT_WARNINGS:NO")

if (CMAKE_BUILD_TESTS)
    set (SECURITY_PSL_FILE "src/test_security.psl.in")
    set (INIT_FILE "src/test_init.yaml.in")
    set (ENTITIES TestEntity TestVfsEntity)

    add_custom_target (${FS_IMAGE}
        COMMAND  ${KL_SDK_ROOT_PATH}/common/prepare_hdd_img.sh
            -f  "ext2"
            -d  ${CMAKE_SOURCE_DIR}/res/hdd/test/
            -s   ${FS_IMG_SIZE}
            -img ${FS_IMAGE})
else ()
    set (SECURITY_PSL_FILE "src/security.psl.in")
    set (INIT_FILE "src/init.yaml.in")
    set (ENTITIES
            FsReaderEntity
            FsWriterEntity
            CredStorageReaderEntity
            FsReaderVfs
            FsWriterVfs
            CredStorageReaderVfs)

    add_custom_target (${FS_IMAGE}
        COMMAND  ${KL_SDK_ROOT_PATH}/common/prepare_hdd_img.sh
            -%1  25
            -%2  25
            -%3  25
            -%4  25
            -f1  "ext2"
            -f2  "ext2"
            -f3  "ext2"
            -f4  "ext2"
            -p1  ${CMAKE_SOURCE_DIR}/res/hdd/p1
            -p2  ${CMAKE_SOURCE_DIR}/res/hdd/p2
            -p3  ${CMAKE_SOURCE_DIR}/res/hdd/p3
            -p4  ${CMAKE_SOURCE_DIR}/res/hdd/p4
            -s   ${FS_IMG_SIZE}
            -img ${FS_IMAGE})
endif ()

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

build_kos_qemu_image (kos-qemu-image
                      EINIT_ENTITY EinitQemu
                      CONNECTIONS_CFG ${INIT_FILE}
                      SECURITY_PSL ${SECURITY_PSL_FILE}
                      QEMU_FLAGS "${QEMU_FLAGS}"
                      IMAGE_FILES ${ENTITIES} ${FILES})

add_dependencies (kos-qemu-image ${FS_IMAGE})

set_target_properties (
    EinitQemu
    PROPERTIES
        LINK_FLAGS ${VULN_LDFLAGS})
