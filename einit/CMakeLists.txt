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
            FsProxy
            FsReaderVfs
            FsWriterVfs)
endif ()

set (FS_IMG_SIZE 64)
if (ROOTFS_SDCARD)
    set (ROOTFS_ENTITY "kl.VfsSdCardFs")
    set (FS_BACKEND ${precompiled_vfsVfsSdCardFs})
    set (FS_IMAGE sdcard0.img)
    set (FS_IMG_FORMAT fat32)
else()
    set (ROOTFS_ENTITY "kl.VfsRamFs")
    set (FS_BACKEND ${precompiled_vfsVfsRamFs})
    set (FS_IMAGE ramdisk0.img)
    set (FS_IMG_FORMAT ext4)
    set (FILES ${CMAKE_BINARY_DIR}/einit/${FS_IMAGE})
endif()

add_custom_target (${FS_IMAGE}
    COMMAND  ${KL_SDK_ROOT_PATH}/common/prepare_hdd_img.sh
        -d   ${CMAKE_SOURCE_DIR}/res/hdd
        -s   ${FS_IMG_SIZE}
        -f   ${FS_IMG_FORMAT}
        -img ${FS_IMAGE})

set (ENTITIES ${ENTITIES} ${FS_BACKEND})

build_kos_hw_image (kos-image
                    EINIT_ENTITY EinitHw
                    CONNECTIONS_CFG ${INIT_FILE}
                    SECURITY_PSL ${SECURITY_PSL_FILE}
                    IMAGE_FILES ${ENTITIES} ${FILES})

if (NOT ROOTFS_SDCARD)
    add_dependencies (kos-image ${FS_IMAGE})
endif ()

set_target_properties (
    EinitHw
    PROPERTIES
        LINK_FLAGS ${VULN_LDFLAGS})
