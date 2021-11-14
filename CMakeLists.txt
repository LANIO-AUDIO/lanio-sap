cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_PARALLEL_LEVEL 8)

project(LanioSAP VERSION 0.1 LANGUAGES CXX)

configure_file(include/version.h.in include/version.h)
configure_file(docker/version.sh.in docker/version.sh)

add_executable("${PROJECT_NAME}"
    src/debug.cpp
    src/application.cpp
    src/sap.cpp
    src/sdp.cpp
    src/main.cpp
    include/sap.hpp # Needed for MOC preprocessing of QObject
    include/application.hpp
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
    "${PROJECT_SOURCE_DIR}/include"
)
target_include_directories("${PROJECT_NAME}" PUBLIC
    "${PROJECT_BINARY_DIR}/include"
)
target_include_directories("${PROJECT_NAME}" SYSTEM PUBLIC
    "${PROJECT_SOURCE_DIR}/dependencies/libsdptransform/include"
)

# Needed for cross-compilation
if(NOT WIN32)
    set(QT_MOC_EXECUTABLE /usr/bin/moc)
    add_executable(Qt5::moc IMPORTED)
    set_property(TARGET Qt5::moc PROPERTY IMPORTED_LOCATION ${QT_MOC_EXECUTABLE})
endif()

find_package(Qt5 COMPONENTS Core Network Sql REQUIRED)

target_link_libraries("${PROJECT_NAME}" PUBLIC
    Qt5::Core
    Qt5::Network
    Qt5::Sql
)