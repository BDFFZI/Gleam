macro(initProject nameSuffix)
    # 获取项目名
    cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH TargetModulePath)
    cmake_path(GET TargetModulePath FILENAME ModuleName)
    set(ProjectName "${ModuleName}${nameSuffix}")

    # 设置项目信息
    set(${ProjectName} TRUE CACHE BOOL "特定项目是否存在")
    set(${ProjectName}Source ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "特定项目的SOURCE目录")
    set(${ProjectName}Binary ${CMAKE_CURRENT_BINARY_DIR} CACHE STRING "特定项目的BINARY目录")

    # 创建项目
    project(${ProjectName})

    # 获取项目文件
    file(GLOB_RECURSE ProjectFiles "*.*")

    # 禁用部分文件的误编译
    set(SpecialFiles ${ProjectFiles})
    list(FILTER SpecialFiles INCLUDE REGEX ".*\.(hlsl|obj)")
    set_property(SOURCE ${SpecialFiles} PROPERTY VS_SETTINGS "ExcludedFromBuild=true")

    # 创建vs筛选器
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES "${ProjectFiles}")

    message("添加项目：${ProjectName}")
endmacro()

# runtime静态库项目
macro(addModule)
    initProject("Runtime")
    add_library(${ProjectName} STATIC "${ProjectFiles}")
endmacro()

# editor静态库项目
macro(addEditor)
    initProject("Editor")
    add_library(${ProjectName} STATIC "${ProjectFiles}")

    # 引用对应的runtime静态库
    linkLibrary(${ModuleName} "Runtime")
endmacro()

# 测试用可执行项目
macro(addTests)
    initProject("Tests")

    # 创建初始化文件，用于初始化依赖的模块
    set(ProjectInitFile "${CMAKE_CURRENT_BINARY_DIR}/__Init__.cpp")
    file(WRITE ${ProjectInitFile} "// ReSharper disable CppUnusedIncludeDirective\n")
    list(APPEND ProjectFiles ${ProjectInitFile})

    add_executable(${ProjectName} "${ProjectFiles}")

    # 引用对应的测试目标库
    linkLibrary(${ModuleName} "Runtime")
    linkLibrary(${ModuleName} "Editor")

    # 引用测试用的基本库（gtest和benchmark）
    find_package(GTest CONFIG REQUIRED)
    target_link_libraries(${ProjectName} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
    find_package(benchmark CONFIG REQUIRED)
    target_link_libraries(${ProjectName} PRIVATE benchmark::benchmark)
endmacro()

# 链接目标库并添加到项目初始化文件
macro(linkLibrary ModuleName SubFolder)
    set(LibraryName "${ModuleName}${SubFolder}")

    # 目标库是否存在
    if(DEFINED ${LibraryName})
        # 存在，添加库依赖
        target_link_libraries(${ProjectName} PRIVATE ${LibraryName})

        # 若项目为可执行项目，还需负责编译库的初始化文件
        get_target_property(ProjectType ${ProjectName} TYPE)

        if(${ProjectType} STREQUAL "EXECUTABLE")
            set(LibraryInitFile "${${LibraryName}Source}/__Init__.h")
            if(EXISTS ${LibraryInitFile})
                file(APPEND ${ProjectInitFile} "#include \"${LibraryInitFile}\"\n")
            endif()
        endif()
    endif()
endmacro()

# 添加模块对应的runtime和editor，并在需要时编写初始化文件自动初始化模块
macro(linkModule ModuleName)
    linkLibrary(${ModuleName} "Runtime")
    linkLibrary(${ModuleName} "Editor")
endmacro()

# 递归添加子目录的函数
macro(add_subdirectories_recursive current_dir)
    file(GLOB children RELATIVE ${current_dir} ${current_dir}/*) # 获取当前目录下的所有文件和文件夹的相对路径

    foreach(child ${children}) # 遍历所有目标
        # 跳过非文件夹
        if(NOT IS_DIRECTORY ${current_dir}/${child})
            continue()
        endif()

        # 计算该项目对应的解决方案文件夹分类
        file(RELATIVE_PATH relative_path ${CMAKE_CURRENT_SOURCE_DIR} ${current_dir}) # 使用相对路径分类
        set(SolutionFolder "LightModules/${relative_path}/${child}")

        set(IsModuleFolder FALSE)

        if(EXISTS "${current_dir}/${child}/Runtime/CMakeLists.txt")
            add_subdirectory(${current_dir}/${child}/Runtime)
            set_target_properties(${child}Runtime PROPERTIES FOLDER ${SolutionFolder})
            set(IsModuleFolder TRUE)
        endif()

        if(EXISTS "${current_dir}/${child}/Tests/CMakeLists.txt")
            add_subdirectory(${current_dir}/${child}/Tests)
            set_target_properties(${child}Tests PROPERTIES FOLDER ${SolutionFolder})
            set(IsModuleFolder TRUE)
        endif()

        if(EXISTS "${current_dir}/${child}/Editor/CMakeLists.txt")
            add_subdirectory(${current_dir}/${child}/Editor)
            set_target_properties(${child}Editor PROPERTIES FOLDER ${SolutionFolder})
            set(IsModuleFolder TRUE)
        endif()

        # 对于其他文件夹需递归检查
        if(NOT IsModuleFolder)
            add_subdirectories_recursive(${current_dir}/${child})
        endif()
    endforeach()
endmacro()

# 从当前目录开始递归添加子目录
add_subdirectories_recursive(${CMAKE_CURRENT_SOURCE_DIR})