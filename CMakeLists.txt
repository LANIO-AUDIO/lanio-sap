cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(LanioSAP VERSION 0.1)

configure_file(lanio-version.h.in lanio-version.h)

add_executable(LanioSAP main.cpp sap.cpp)

add_subdirectory("${PROJECT_SOURCE_DIR}/dependencies/libsdptransform")

target_link_libraries(LanioSAP PUBLIC
    pthread
    sdptransform
)

target_include_directories(LanioSAP PUBLIC
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/dependencies/libsdptransform/include"
)