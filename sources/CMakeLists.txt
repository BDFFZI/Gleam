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
    set(SpecialFiles ${ProjectFiles})
    list(FILTER SpecialFiles INCLUDE REGEX ".*\.(hlsl|obj)")
    set_property(SOURCE ${SpecialFiles} PROPERTY VS_SETTINGS "ExcludedFromBuild=true")

    # 创建vs筛选器
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES "${ProjectFiles}")

    message("添加项目：${ProjectName}")
endmacro()

# runtime静态库
macro(addModule)
    initProject("")
    add_library(${ProjectName} STATIC "${ProjectFiles}")
endmacro()

# editor静态库
macro(addEditor)
    initProject("Tests")
    add_library(${ProjectName} STATIC "${ProjectFiles}")

    # 引用编辑器的目标模块
    target_link_libraries(${ProjectName} PRIVATE ${ModuleName})
endmacro()

# 添加模块对应的runtime和editor，并在需要时编写初始化文件自动初始化模块
macro(linkModule ModuleName)
    get_target_property(ProjectType ${ProjectName} TYPE)

    target_link_libraries(${ProjectName} PRIVATE "${ModuleName}")

    if(${ProjectType} STREQUAL "EXECUTABLE")
        file(APPEND ${ModuleInitFile} "#include \"${CMAKE_SOURCE_DIR}/sources/${ModuleName}/Runtime/__Module__.h\"\n")
    endif()

    if(${ModuleName}Editor)
        target_link_libraries(${ProjectName} PRIVATE "${ModuleName}Editor")

        if(${ProjectType} STREQUAL "EXECUTABLE")
            file(APPEND ${ModuleInitFile} "#include \"${CMAKE_SOURCE_DIR}/sources/${ModuleName}/Editor/__Module__.h\"\n")
        endif()
    endif()


endmacro()

macro(addTests)
    initProject("Tests")

    # 添加模块初始化文件
    set(ModuleInitFile "${CMAKE_CURRENT_BINARY_DIR}/__ModuleInit__.cpp")
    file(WRITE ${ModuleInitFile} "// ReSharper disable CppUnusedIncludeDirective\n")
    list(APPEND ProjectFiles ${ModuleInitFile})

    add_executable(${ProjectName} "${ProjectFiles}")

    # 引用测试的目标模块
    linkModule(${ModuleName})

    # 引用测试用的基本库（gtest和benchmark）
    find_package(GTest CONFIG REQUIRED)
    target_link_libraries(${ProjectName} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
    find_package(benchmark CONFIG REQUIRED)
    target_link_libraries(${ProjectName} PRIVATE benchmark::benchmark)
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