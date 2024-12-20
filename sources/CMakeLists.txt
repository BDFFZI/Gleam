macro(initProject nameSuffix)
    # 获取项目名
    cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH TargetModulePath)
    cmake_path(GET TargetModulePath FILENAME ModuleName)
    set(ProjectName "${ModuleName}${nameSuffix}")
    # 设置项目创建标记
    set(${ProjectName} TRUE)
    # 创建项目
    project(${ProjectName})
    # 获取项目文件
    file(GLOB_RECURSE ProjectFiles "*.*")
    # 禁用部分文件的误编译
    set(ShaderFiles ${ProjectFiles})
    list(FILTER ShaderFiles INCLUDE REGEX ".*\.(hlsl|obj)")
    set_source_files_properties(${ShaderFiles} PROPERTIES VS_TOOL_OVERRIDE "None")
    # 创建vs筛选器
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES "${ProjectFiles}")

    message("添加项目：${ProjectName}")
endmacro()

macro(addModule)
    initProject("")
    add_library(${ProjectName} STATIC "${ProjectFiles}")
endmacro()

macro(addTests)
    initProject("Tests")
    add_executable(${ProjectName} "${ProjectFiles}")

    # 引用测试的目标模块
    target_link_libraries(${ProjectName} PRIVATE ${ModuleName})

    # 引用测试用的基本库（gtest和benchmark）
    find_package(GTest CONFIG REQUIRED)
    target_link_libraries(${ProjectName} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
    find_package(benchmark CONFIG REQUIRED)
    target_link_libraries(${ProjectName} PRIVATE benchmark::benchmark)
endmacro()

macro(addEditor)
    initProject("Tests")
    add_library(${ProjectName} STATIC "${ProjectFiles}")

    # 引用编辑器的目标模块
    target_link_libraries(${ProjectName} PRIVATE ${ModuleName})
endmacro()

macro(linkModule ModuleName)
    target_link_libraries(${ProjectName} PRIVATE "${ModuleName}")

    if(${ModuleName}Editor)
        target_link_libraries(${ProjectName} PRIVATE "${ModuleName}Editor")
    endif()
endmacro()

# 递归添加子目录的函数
function(add_subdirectories_recursive current_dir)
    file(GLOB children RELATIVE ${current_dir} ${current_dir}/*) # 获取当前目录下的所有文件和文件夹的相对路径

    foreach(child ${children}) # 遍历所有目标
        # 跳过非文件夹
        if(NOT IS_DIRECTORY ${current_dir}/${child})
            continue()
        endif()

        # 计算该项目对应的解决方案文件夹分类
        file(RELATIVE_PATH relative_path ${CMAKE_CURRENT_SOURCE_DIR} ${current_dir}) # 使用相对路径分类
        set(SolutionFolder "LightModules/${relative_path}/${child}")

        if(EXISTS "${current_dir}/${child}/Runtime/CMakeLists.txt")
            add_subdirectory(${current_dir}/${child}/Runtime)
            set_target_properties(${child} PROPERTIES FOLDER ${SolutionFolder})

            if(EXISTS "${current_dir}/${child}/Tests/CMakeLists.txt")
                add_subdirectory(${current_dir}/${child}/Tests)
                set_target_properties(${child}Tests PROPERTIES FOLDER ${SolutionFolder})
            endif()

            if(EXISTS "${current_dir}/${child}/Editor/CMakeLists.txt")
                add_subdirectory(${current_dir}/${child}/Editor)
                set_target_properties(${child}Editor PROPERTIES FOLDER ${SolutionFolder})
            endif()

            continue()
        endif()

        # 对于其他文件夹需递归检查
        add_subdirectories_recursive(${current_dir}/${child})
    endforeach()
endfunction()

# 从当前目录开始递归添加子目录
add_subdirectories_recursive(${CMAKE_CURRENT_SOURCE_DIR})