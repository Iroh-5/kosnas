project (TestEntity)

include (vfs_server/custom_vfs_server)

find_package (GTest REQUIRED)

set (SOURCES
    fs_test_common.cpp
    fs_reader_test.cpp
    fs_writer_test.cpp
    cred_storage_reader_test.cpp)

set (LIBS GTest::gtest_main FsReaderLib FsWriterLib FsCommonLib CredStorageReaderLib)

nk_build_edl_files (test_edl NK_MODULE "nas" EDL "${CMAKE_SOURCE_DIR}/res/TestEntity.edl")

set (VFS_SERVER_TARGET TestVfsEntity)
set (VFS_SERVER_PART 0)

create_custom_vfs_server (${VFS_SERVER_TARGET} ${VFS_SERVER_PART})

add_executable             (${PROJECT_NAME} ${SOURCES} ${EDL_FILES})
add_dependencies           (${PROJECT_NAME} ${VFS_SERVER_TARGET} test_edl)
target_compile_features    (${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries      (${PROJECT_NAME} PRIVATE ${LIBS})

target_include_directories (
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/fs_service
        ${CMAKE_SOURCE_DIR}/fs_service/common/include
        ${CMAKE_SOURCE_DIR}/auth_service)

set_target_properties (
    ${PROJECT_NAME}
    PROPERTIES
        LINK_FLAGS ${VULN_LDFLAGS})
