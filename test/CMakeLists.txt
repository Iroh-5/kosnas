project (TestEntity)

find_package (GTest REQUIRED)

set (SOURCES fs_manager_test.cpp)
set (LIBS GTest::gtest_main FsManager)

nk_build_edl_files (test_edl NK_MODULE "nas" EDL "${CMAKE_SOURCE_DIR}/res/TestEntity.edl")

add_executable             (${PROJECT_NAME} ${SOURCES} ${EDL_FILES})
add_dependencies           (${PROJECT_NAME} test_edl)
target_compile_features    (${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries      (${PROJECT_NAME} PRIVATE ${LIBS})
target_include_directories (${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/fs/lib/include)

set_target_properties (
    ${PROJECT_NAME}
    PROPERTIES
        LINK_FLAGS ${VULN_LDFLAGS})
