project (FsCommon)

set (LIB_NAME ${PROJECT_NAME}Lib)

add_library                (${LIB_NAME} STATIC source/fs_base.cpp)
target_compile_features    (${LIB_NAME} PRIVATE cxx_std_17)
target_link_libraries      (${LIB_NAME} PUBLIC ${vfs_CLIENT_LIB} fmt::fmt)
target_include_directories (${LIB_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
