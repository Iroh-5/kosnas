project (GatewayEntity)

nk_build_edl_files (
    gateway_edl
    NK_MODULE "nas"
    EDL "${CMAKE_SOURCE_DIR}/res/GatewayEntity.edl")

set (SOURCES
    source/auth_service_proxy.cpp
    source/main.cpp)

add_executable          (${PROJECT_NAME} ${SOURCES} ${EDL_FILES})
add_dependencies        (${PROJECT_NAME} gateway_edl)
target_compile_features (${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries   (${PROJECT_NAME} PRIVATE Common)

target_include_directories (
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/gateway/include
        ${CMAKE_SOURCE_DIR}/common/include)

set_target_properties (
    ${PROJECT_NAME}
    PROPERTIES
        LINK_FLAGS ${VULN_LDFLAGS})
