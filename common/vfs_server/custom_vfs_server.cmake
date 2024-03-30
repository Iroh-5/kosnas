function (create_custom_vfs_server TARGET_NAME PARTITION)
    nk_build_edl_files (
        ${TARGET_NAME}_edl
        NK_MODULE "nas"
        EDL "${CMAKE_SOURCE_DIR}/res/${TARGET_NAME}.edl"
        NK_FLAGS "--extended-errors")

    set (VFS_SERVER_SOURCES_DIR "${CMAKE_SOURCE_DIR}/common/vfs_server/source/")

    add_executable             (${TARGET_NAME} "${VFS_SERVER_SOURCES_DIR}/main.cpp")
    add_dependencies           (${TARGET_NAME} ${TARGET_NAME}_edl)
    target_compile_features    (${TARGET_NAME} PRIVATE cxx_std_17)
    target_compile_definitions (${TARGET_NAME} PRIVATE PARTITION=${PARTITION})
    set_target_properties      (${TARGET_NAME} PROPERTIES LINK_FLAGS ${VULN_LDFLAGS})

    target_link_libraries (
        ${TARGET_NAME}
        PRIVATE
            ${vfs_SERVER_LIB}
            ${vfs_FS_LIB}
            ${blkdev_CLIENT_LIB}
            ${LWEXT4_LIB}
            fmt::fmt)
endfunction ()
