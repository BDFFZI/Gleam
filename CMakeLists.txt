cmake_minimum_required(VERSION 3.30.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output")
add_compile_options("/utf-8")

file(GLOB Sources "${CMAKE_SOURCE_DIR}/sources/*")

foreach(Source ${Sources})
    if(IS_DIRECTORY ${Source})
        include_directories(${Source})
    endif()
endforeach()

project(Light)
include(CreateProject.cmake)

# 同步所有Assets文件夹
file(GLOB_RECURSE ResourcesPaths LIST_DIRECTORIES true "sources/[!.]" "samples/[!.]")
list(FILTER ResourcesPaths INCLUDE REGEX "Resources$")

foreach(ResourcesPath ${ResourcesPaths})
    # string(REPLACE ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} NewAssetPath ${ResourcesPath})
    # file(COPY ${ResourcesPath} DESTINATION ${NewAssetPath}/..)
    file(COPY ${ResourcesPath} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    message("复制资源：${ResourcesPath}")
endforeach()

# 运行子目录中的所有CMakeLists.txt
function(add_subdirectories_recursive current_dir)
    file(GLOB children RELATIVE ${current_dir} ${current_dir}/*) # 获取当前目录下的所有文件和文件夹的相对路径

    foreach(child ${children}) # 遍历所有目标
        set(DirectoryPath "${current_dir}/${child}")

        # 跳过非文件夹
        if(NOT IS_DIRECTORY ${DirectoryPath})
            continue()
        endif()

        if(EXISTS "${DirectoryPath}/CMakeLists.txt")
            # 添加目录
            add_subdirectory(${DirectoryPath})

        else()
            # 递归检查目录
            add_subdirectories_recursive(${current_dir}/${child})
        endif()
    endforeach()
endfunction()

add_subdirectories_recursive(${CMAKE_CURRENT_SOURCE_DIR})

# # 后处理项目
# foreach(ProjectName ${Projects})
#     get_target_property(ProjectType ${ProjectName} TYPE)

#     if(ProjectType STREQUAL "EXECUTABLE")
#         get_target_property(Libraries ${ProjectName} LINK_LIBRARIES)

#         foreach(Library ${Libraries})
#             if(Library MATCHES "Light.*")
#                 message("${ProjectName}：USE_${Library}")
#                 target_compile_definitions(${ProjectName} PRIVATE "USE_${Library}")
#             endif()
#         endforeach()
#     endif()
# endforeach()
