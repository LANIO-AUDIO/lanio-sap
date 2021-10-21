cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

project(LanioSAP VERSION 0.1)

configure_file(src/lanio-version.h.in lanio-version.h)

add_executable("${PROJECT_NAME}" src/main.cpp src/sap.cpp)

if(MSVC)
    #target_compile_options(${PROJECT_NAME} PRIVATE
    #    /Wall /WX
    #)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Werror -Weffc++ -Wsign-conversion -pedantic-errors
    )
    target_link_libraries("${PROJECT_NAME}" PUBLIC
        pthread
    )
endif()

add_subdirectory("${PROJECT_SOURCE_DIR}/dependencies/libsdptransform")

target_link_libraries("${PROJECT_NAME}" PUBLIC
    sdptransform
)

target_include_directories("${PROJECT_NAME}" PUBLIC
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/dependencies/libsdptransform/include"
    "${PROJECT_SOURCE_DIR}/dependencies/boost/asio/include"
    "${PROJECT_SOURCE_DIR}/dependencies/boost/bind/include"
)