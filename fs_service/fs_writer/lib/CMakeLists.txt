project (FsWriterLib)

add_library                (${PROJECT_NAME} STATIC source/fs_writer.cpp)
target_compile_features    (${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries      (${PROJECT_NAME} PUBLIC FsCommonLib ${vfs_CLIENT_LIB} fmt::fmt)

target_include_directories (
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/fs_service/common/include)

set_target_properties (
    ${PROJECT_NAME}
    PROPERTIES
        ${vfs_ENTITY}_REPLACEMENT ""
        LINK_FLAGS ${VULN_LDFLAGS})
