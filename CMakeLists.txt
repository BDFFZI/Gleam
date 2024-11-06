cmake_minimum_required(VERSION 3.30.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
add_compile_options("/utf-8")
include_directories("${CMAKE_SOURCE_DIR}/sources")

project(Light)
add_subdirectory("sources")
add_subdirectory("samples")

# 同步所有Assets文件夹
file(GLOB_RECURSE AssetPaths LIST_DIRECTORIES true "sources/[!.]" "samples/[!.]")
list(FILTER AssetPaths INCLUDE REGEX "Assets$")
foreach(AssetPath ${AssetPaths})
    string(REPLACE ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} NewAssetPath ${AssetPath})
    file(COPY ${AssetPath} DESTINATION ${NewAssetPath}/..)
    message("复制资源：${NewAssetPath}")
endforeach()