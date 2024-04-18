project (CryptoModuleEntity)

nk_build_idl_files (
    crypto_module_idl
    NK_MODULE "nas"
    IDL "${CMAKE_SOURCE_DIR}/res/CryptoModule.idl")
add_dependencies (crypto_module_idl common_types_idl)

nk_build_cdl_files (
    crypto_module_cdl
    IDL_TARGET crypto_module_idl
    NK_MODULE "nas"
    CDL "${CMAKE_SOURCE_DIR}/res/CryptoModule.cdl")

nk_build_edl_files (
    crypto_module_edl
    CDL_TARGET crypto_module_cdl
    NK_MODULE "nas"
    EDL "${CMAKE_SOURCE_DIR}/res/CryptoModuleEntity.edl")

add_executable          (${PROJECT_NAME} source/entity.cpp source/main.cpp ${EDL_FILES})
add_dependencies        (${PROJECT_NAME} crypto_module_edl)
target_compile_features (${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries   (${PROJECT_NAME} picosha2)

target_include_directories (
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/auth_service/crypto_module/entity/include)

set_target_properties (
    ${PROJECT_NAME}
    PROPERTIES
        LINK_FLAGS ${VULN_LDFLAGS})
