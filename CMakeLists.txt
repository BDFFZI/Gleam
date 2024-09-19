cmake_minimum_required(VERSION 3.30)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
add_compile_options("/utf-8")
include_directories("${CMAKE_SOURCE_DIR}/sources")

project(Light)
add_subdirectory("sources")
add_subdirectory("samples")