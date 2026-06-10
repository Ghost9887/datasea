cmake_minimum_required(VERSION 3.10)
project(datasea LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

find_package(Catch2 3 REQUIRED)

file(GLOB_RECURSE LIB_SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cc
)

list(REMOVE_ITEM LIB_SOURCES "${CMAKE_SOURCE_DIR}/src/main.cc")

file(GLOB_RECURSE TESTS
    ${CMAKE_SOURCE_DIR}/tests/*.cc
)

add_library(datasea_lib ${LIB_SOURCES})

target_include_directories(datasea_lib
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

add_executable(datasea src/main.cc)
target_link_libraries(datasea PRIVATE datasea_lib)

add_executable(datasea_tests ${TESTS})
target_link_libraries(datasea_tests PRIVATE datasea_lib Catch2::Catch2WithMain)

target_compile_options(datasea PRIVATE -Wall -Wextra -Wpedantic)
