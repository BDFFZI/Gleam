cmake_minimum_required(VERSION 3.30.0)

# 使用C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# 代码文件使用utf-8
add_compile_options("/utf-8")

# 自定义输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output")
# 设置头文件目录，使可以从每个项目的父目录包含项目文件
file(GLOB Sources "${CMAKE_SOURCE_DIR}/sources/*")

foreach(Source ${Sources})
    if(IS_DIRECTORY ${Source})
        include_directories(${Source})
    endif()
endforeach()

project(Light)
include(CreateProject.cmake)

# 同步所有Resources文件夹
# file(GLOB_RECURSE ResourcesPaths LIST_DIRECTORIES true "sources/[!.]" "samples/[!.]")
# list(FILTER ResourcesPaths INCLUDE REGEX "Resources$")
# foreach(ResourcesPath ${ResourcesPaths})
# file(COPY ${ResourcesPath} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
# message("复制资源：${ResourcesPath}")
# endforeach()

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

set(AllExecutable "" CACHE STRING "全部可执行项目" FORCE)
add_subdirectories_recursive(${CMAKE_CURRENT_SOURCE_DIR})

message("后处理可执行文件 ${AllExecutable}")

foreach(Executable ${AllExecutable})
    set(ALLResourcesPaths "")

    foreach(dependency ${${Executable}Dependencies})
        # 查看依赖的Resources文件
        file(GLOB_RECURSE ResourcesPaths LIST_DIRECTORIES true "${${dependency}Source}/[!.]")
        list(FILTER ResourcesPaths INCLUDE REGEX "Resources$")
        list(APPEND ALLResourcesPaths ${ResourcesPaths})
    endforeach()

    message("发现资源依赖：${Executable}->${ALLResourcesPaths}")
    add_custom_command(
        TARGET ${Executable}
        POST_BUILD
        COMMAND cmake -D ResourcesPath=${ALLResourcesPaths} -P ${CMAKE_SOURCE_DIR}/CopyResources.cmake
    )
endforeach()