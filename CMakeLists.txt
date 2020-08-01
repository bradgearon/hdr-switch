cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)

project(hdr-switch)

add_subdirectory(src/core)
add_subdirectory(src/service)
add_subdirectory(src/tray)