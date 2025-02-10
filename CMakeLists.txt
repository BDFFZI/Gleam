cmake_minimum_required(VERSION 3.30.0)
project(Light)

# 全局项目设置
set(CMAKE_CXX_STANDARD 20) # 使用C++20
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
add_compile_options("/utf-8") # 代码文件使用utf-8
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output") # 自定义输出目录

file(GLOB Sources "${CMAKE_SOURCE_DIR}/sources/*")

foreach(Source ${Sources})
    if(IS_DIRECTORY ${Source})
        include_directories(${Source}) # 设置头文件目录，使可以从每个项目的父目录包含项目文件
    endif()
endforeach()

include(ProjectCreate.cmake) # 供子目录使用的项目创建宏

# 运行子目录中的所有CMakeLists.txt
function(add_subdirectories_recursive current_dir)
    file(GLOB children ${current_dir}/*) # 获取当前目录所有子目录或文件路径

    foreach(child ${children}) # 遍历所有子文件路径
        # 跳过非文件夹
        if(NOT IS_DIRECTORY ${child})
            continue()
        endif()

        if(EXISTS "${child}/CMakeLists.txt")
            # 添加cmake项目
            add_subdirectory(${child})
        else()
            # 递归检查目录
            add_subdirectories_recursive(${child})
        endif()
    endforeach()
endfunction()

set(AllProjects "" CACHE STRING "全部项目" FORCE)
add_subdirectories_recursive(${CMAKE_CURRENT_SOURCE_DIR})
list(REMOVE_DUPLICATES AllProjects)

# 后处理项目
include(ProjectPostProcess.cmake)

# 额外的打包功能
include(ProjectPack.cmake)