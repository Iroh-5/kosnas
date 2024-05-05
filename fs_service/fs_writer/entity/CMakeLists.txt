project (FsWriterEntity)

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

add_executable          (${PROJECT_NAME} source/entity.cpp source/main.cpp ${EDL_FILES})
add_dependencies        (${PROJECT_NAME} ${VFS_SERVER_TARGET} fs_writer_edl)
target_compile_features (${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries   (${PROJECT_NAME} FsWriterLib Common)

target_include_directories (
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/common/include
        ${CMAKE_SOURCE_DIR}/fs_service/common/include
        ${CMAKE_SOURCE_DIR}/fs_service/fs_writer/lib/include
        ${CMAKE_SOURCE_DIR}/fs_service/fs_writer/entity/include)

set_target_properties (
    ${PROJECT_NAME}
    PROPERTIES
        LINK_FLAGS ${VULN_LDFLAGS})
