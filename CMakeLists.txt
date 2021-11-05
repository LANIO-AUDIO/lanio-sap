cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_PREFIX_PATH "C:\\Qt\\5.15.2\\mingw81_64")

project(LanioSAP VERSION 0.1 LANGUAGES CXX)

configure_file(src/lanio-version.h.in lanio-version.h)

add_executable("${PROJECT_NAME}"
    src/main.cpp
    src/sap.cpp
    src/sdp.cpp
)

set_property(TARGET ${PROJECT_NAME} PROPERTY AUTOMOC ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY AUTORCC ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY AUTOUIC ON)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4 /WX
    )
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
)
target_include_directories("${PROJECT_NAME}" SYSTEM PUBLIC
    "${PROJECT_SOURCE_DIR}/dependencies/libsdptransform/include"
)

find_package(Qt5 COMPONENTS Core Network Sql REQUIRED)

target_link_libraries("${PROJECT_NAME}" PUBLIC
    Qt5::Core
    Qt5::Network
    Qt5::Sql
)