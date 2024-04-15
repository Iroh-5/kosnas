project (common)

set (SOURCES source/utils.cpp)

add_library             (${PROJECT_NAME} STATIC ${SOURCES})
target_compile_features (${PROJECT_NAME} PRIVATE cxx_std_17)

target_include_directories (
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties (
    ${PROJECT_NAME}
    PROPERTIES
        ${vfs_ENTITY}_REPLACEMENT ""
        LINK_FLAGS ${VULN_LDFLAGS})
