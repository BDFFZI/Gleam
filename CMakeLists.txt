cmake_minimum_required(VERSION 3.30.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output")
add_compile_options("/utf-8")
include_directories("${CMAKE_SOURCE_DIR}/sources/libraries")
include_directories("${CMAKE_SOURCE_DIR}/sources/Modules")

project(Light)
add_subdirectory("sources")
add_subdirectory("samples")

# 同步所有Assets文件夹
file(GLOB_RECURSE ResourcesPaths LIST_DIRECTORIES true "sources/[!.]" "samples/[!.]")
list(FILTER ResourcesPaths INCLUDE REGEX "Resources$")
foreach(ResourcesPath ${ResourcesPaths})
    # string(REPLACE ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} NewAssetPath ${ResourcesPath})
    # file(COPY ${ResourcesPath} DESTINATION ${NewAssetPath}/..)
    file(COPY ${ResourcesPath} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    message("复制资源：${ResourcesPath}")
endforeach()