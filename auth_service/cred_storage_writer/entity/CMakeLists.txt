project (CredStorageWriterEntity)

include (vfs_server/custom_vfs_server)

nk_build_idl_files (
    cred_storage_writer_idl
    NK_MODULE "nas"
    IDL "${CMAKE_SOURCE_DIR}/res/CredStorageWriter.idl")
add_dependencies (cred_storage_writer_idl common_types_idl)

nk_build_cdl_files (
    cred_storage_writer_cdl
    IDL_TARGET cred_storage_writer_idl
    NK_MODULE "nas"
    CDL "${CMAKE_SOURCE_DIR}/res/CredStorageWriter.cdl")

nk_build_edl_files (
    cred_storage_writer_edl
    CDL_TARGET cred_storage_writer_cdl
    NK_MODULE "nas"
    EDL "${CMAKE_SOURCE_DIR}/res/CredStorageWriterEntity.edl")

set (VFS_SERVER_TARGET CredStorageWriterVfs)
set (VFS_SERVER_PART 3)

create_custom_vfs_server (${VFS_SERVER_TARGET} ${VFS_SERVER_PART})

add_executable          (${PROJECT_NAME} source/entity.cpp source/main.cpp ${EDL_FILES})
add_dependencies        (${PROJECT_NAME} ${VFS_SERVER_TARGET} cred_storage_writer_edl)
target_compile_features (${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries   (${PROJECT_NAME} CredStorageWriterLib)

target_include_directories (
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/auth_service/cred_storage_writer/lib/include
        ${CMAKE_SOURCE_DIR}/auth_service/cred_storage_writer/entity/include)

set_target_properties (
    ${PROJECT_NAME}
    PROPERTIES
        LINK_FLAGS ${VULN_LDFLAGS})
