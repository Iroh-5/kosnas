project (CredStorageReaderEntity)

nk_build_idl_files (
    cred_storage_reader_idl
    NK_MODULE "nas"
    IDL "${CMAKE_SOURCE_DIR}/res/CredStorageReader.idl")
add_dependencies (cred_storage_reader_idl common_types_idl)

nk_build_cdl_files (
    cred_storage_reader_cdl
    IDL_TARGET cred_storage_reader_idl
    NK_MODULE "nas"
    CDL "${CMAKE_SOURCE_DIR}/res/CredStorageReader.cdl")

nk_build_edl_files (
    cred_storage_reader_edl
    CDL_TARGET cred_storage_reader_cdl
    NK_MODULE "nas"
    EDL "${CMAKE_SOURCE_DIR}/res/CredStorageReaderEntity.edl")

add_executable          (${PROJECT_NAME} source/entity.cpp source/main.cpp ${EDL_FILES})
add_dependencies        (${PROJECT_NAME} ${VFS_SERVER_TARGET} cred_storage_reader_edl)
target_compile_features (${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries   (${PROJECT_NAME} CredStorageReaderLib Common)

target_include_directories (
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/common/include
        ${CMAKE_SOURCE_DIR}/auth_service/cred_storage_reader/lib/include
        ${CMAKE_SOURCE_DIR}/auth_service/cred_storage_reader/entity/include)

set_target_properties (
    ${PROJECT_NAME}
    PROPERTIES
        LINK_FLAGS ${VULN_LDFLAGS})
