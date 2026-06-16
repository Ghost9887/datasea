cmake_minimum_required(VERSION 3.10)
project(datasea LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

file(GLOB_RECURSE APP_SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cc
)

add_executable(datasea ${APP_SOURCES})

target_include_directories(datasea
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_compile_options(datasea PRIVATE -Wall -Wextra -Wpedantic)

configure_file(
    ${CMAKE_SOURCE_DIR}/config.h.in
    ${CMAKE_BINARY_DIR}/config.h
)

target_include_directories(datasea PRIVATE ${CMAKE_BINARY_DIR})

install(TARGETS datasea RUNTIME DESTINATION bin)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/data
        DESTINATION share/datasea)
