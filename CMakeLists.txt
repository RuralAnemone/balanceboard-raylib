cmake_minimum_required(VERSION 3.10)
project(demo)

add_executable(demo demo.c)

find_library(WIIUSE_LIB wiiuse)
find_library(RAYLIB_LIB raylib)

target_link_libraries(demo ${WIIUSE_LIB} ${RAYLIB_LIB} m bluetooth)