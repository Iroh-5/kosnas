project (FsWriterEntity)

nk_build_edl_files (fs_writer_edl NK_MODULE "nas" EDL "${CMAKE_SOURCE_DIR}/res/FsWriter.edl")

add_executable             (${PROJECT_NAME} source/entity.cpp ${EDL_FILES})
add_dependencies           (${PROJECT_NAME} fs_writer_edl)
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
