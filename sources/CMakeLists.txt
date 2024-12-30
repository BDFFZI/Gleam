function(getProjectName in_path out_projectName)
    file(RELATIVE_PATH relative_path "${CMAKE_SOURCE_DIR}/sources" ${in_path})
    string(REPLACE "/" "" projectName ${relative_path})
    set(${out_projectName} ${projectName} PARENT_SCOPE)
endfunction()

macro(initProject)
    # 获取项目名
    getProjectName(${CMAKE_CURRENT_SOURCE_DIR} ProjectName)
    message("添加项目：${ProjectName}")

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
endmacro()

# 库项目
macro(setLibrary)
    add_library(${ProjectName} STATIC "${ProjectFiles}")
endmacro()

# 可执行文件项目
macro(setExecutable)
    # 创建初始化文件，用于初始化依赖库
    set(ProjectInitFile "${CMAKE_CURRENT_BINARY_DIR}/__Init__.cpp")
    file(WRITE ${ProjectInitFile} "// ReSharper disable CppUnusedIncludeDirective\n")
    list(APPEND ProjectFiles ${ProjectInitFile})

    add_executable(${ProjectName} "${ProjectFiles}")
endmacro()

macro(addProject)
    initProject()

    if(ProjectName MATCHES ".*Tests.*")
        setExecutable()

        # 引用测试用的基本库（gtest和benchmark）
        find_package(GTest CONFIG REQUIRED)
        target_link_libraries(${ProjectName} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
        find_package(benchmark CONFIG REQUIRED)
        target_link_libraries(${ProjectName} PRIVATE benchmark::benchmark)

        # 引用测试目标库
        string(REPLACE "Tests" "" TargetLibrary ${ProjectName})
        linkLibrary(${TargetLibrary})
    else()
        setLibrary()

        if(ProjectName MATCHES ".*Editor.*")
            # 引用运行时库
            string(REPLACE "Editor" "Runtime" TargetLibrary ${ProjectName})
            linkLibrary(${TargetLibrary})
        endif()
    endif()

    # 分类到解决方案目录
    cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH ParentPath)
    file(RELATIVE_PATH relative_path "${CMAKE_SOURCE_DIR}/sources" ${ParentPath}) # 使用相对路径分类
    set(SolutionFolder "LightSources/${relative_path}")
    set_target_properties(${ProjectName} PROPERTIES FOLDER ${SolutionFolder})

    # 简化项目名称（不知道为什么没有效果）
    # cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME DirectoryName)
    # set_target_properties(${ProjectName} PROPERTIES OUTPUT_NAME ${DirectoryName})
endmacro()

# 链接目标库并在需要时自动保护库初始化文件
macro(linkLibrary LibraryName)
    target_link_libraries(${ProjectName} PRIVATE ${LibraryName})

    # 若项目为可执行项目，还需负责编译库的初始化文件
    get_target_property(ProjectType ${ProjectName} TYPE)

    if(${ProjectType} STREQUAL "EXECUTABLE")
        set(LibraryInitFile "${${LibraryName}Source}/__Init__.h")
        if(EXISTS ${LibraryInitFile})
            file(APPEND ${ProjectInitFile} "#include \"${LibraryInitFile}\"\n")
        endif()
    endif()
endmacro()

# 递归添加子目录的函数
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

# 从当前目录开始递归添加子目录
add_subdirectories_recursive(${CMAKE_CURRENT_SOURCE_DIR})