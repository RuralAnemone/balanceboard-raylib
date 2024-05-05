cmake_minimum_required(VERSION 3.10)
project(demo)

add_executable(demo demo.c)
add_executable(ir-demo ir.c)

find_library(WIIUSE_LIB wiiuse)
find_library(RAYLIB_LIB raylib)

target_link_libraries(demo ${WIIUSE_LIB} ${RAYLIB_LIB} m bluetooth)
target_link_libraries(ir-demo ${WIIUSE_LIB} ${RAYLIB_LIB} m bluetooth)
