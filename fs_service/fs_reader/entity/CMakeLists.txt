project (FsReaderEntity)

include (vfs_server/custom_vfs_server)

nk_build_edl_files (fs_reader_edl NK_MODULE "nas" EDL "${CMAKE_SOURCE_DIR}/res/FsReader.edl")

set (VFS_SERVER_TARGET FsReaderVfs)
set (VFS_SERVER_PART 0)

create_custom_vfs_server (${VFS_SERVER_TARGET} ${VFS_SERVER_PART})

add_executable             (${PROJECT_NAME} source/entity.cpp ${EDL_FILES})
add_dependencies           (${PROJECT_NAME} ${VFS_SERVER_TARGET} fs_reader_edl)
target_compile_features    (${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries      (${PROJECT_NAME} FsReaderLib)

target_include_directories (
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/fs_service/fs_reader/lib/include
        ${CMAKE_SOURCE_DIR}/fs_service/fs_reader/entity/include)

set_target_properties (
    ${PROJECT_NAME}
    PROPERTIES
        LINK_FLAGS ${VULN_LDFLAGS})
