cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(SimpleMacro VERSION 1.0.0)

file(GLOB SOURCES
    src/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Impossible de trouver le Geode SDK ! Definissez la variable d'environnement GEODE_SDK.")
else()
    message(STATUS "Geode SDK trouve : $ENV{GEODE_SDK}")
    add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)
endif()

setup_geode_mod(${PROJECT_NAME})
