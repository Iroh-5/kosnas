project (FsWriterEntity)

include (vfs_server/custom_vfs_server)

nk_build_idl_files (
    fs_writer_idl
    NK_MODULE "nas"
    IDL "${CMAKE_SOURCE_DIR}/res/FsWriter.idl")
add_dependencies (fs_writer_idl common_types_idl)

nk_build_cdl_files (
    fs_writer_cdl
    IDL_TARGET fs_writer_idl
    NK_MODULE "nas"
    CDL "${CMAKE_SOURCE_DIR}/res/FsWriter.cdl")

nk_build_edl_files (
    fs_writer_edl
    CDL_TARGET fs_writer_cdl
    NK_MODULE "nas"
    EDL "${CMAKE_SOURCE_DIR}/res/FsWriterEntity.edl")

set (VFS_SERVER_TARGET FsWriterVfs)
set (VFS_SERVER_PART 1)

create_custom_vfs_server (${VFS_SERVER_TARGET} ${VFS_SERVER_PART})

add_executable             (${PROJECT_NAME} source/entity.cpp ${EDL_FILES})
add_dependencies           (${PROJECT_NAME} ${VFS_SERVER_TARGET} fs_writer_edl)
target_compile_features    (${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries      (${PROJECT_NAME} FsWriterLib)

target_include_directories (
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/fs_service/fs_reader/lib/include
        ${CMAKE_SOURCE_DIR}/fs_service/fs_reader/entity/include)

set_target_properties (
    ${PROJECT_NAME}
    PROPERTIES
        LINK_FLAGS ${VULN_LDFLAGS})
