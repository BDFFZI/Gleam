function(parseRelativeDir in_path out_dir)
    file(RELATIVE_PATH relative_path "${CMAKE_SOURCE_DIR}/sources" ${in_path})
    set(${out_dir} ${relative_path} PARENT_SCOPE)
endfunction()

function(parseName in_path out_projectName)
    parseRelativeDir(${in_path} relative_path)
    string(REPLACE "/" "" projectName ${relative_path})
    set(${out_projectName} ${projectName} PARENT_SCOPE)
endfunction()

# 创建一个新项目
macro(addProject subName)
    # 获取路径和名称信息
    set(ProjectSource "${PackageSource}/${subName}")
    set(ProjectBinary "${PackageBinary}/${subName}")
    set(ProjectName "${PackageName}${subName}")
    message("添加项目：${ProjectName}")

    # 存储项目信息
    set(${ProjectName} TRUE CACHE BOOL "特定项目是否存在")
    set(${ProjectName}Source ${ProjectSource} CACHE STRING "特定项目的SOURCE目录")
    set(${ProjectName}Binary ${ProjectBinary} CACHE STRING "特定项目的SOURCE目录")

    # 创建项目
    project(${ProjectName})

    # 获取项目文件
    file(GLOB_RECURSE ProjectFiles "${ProjectSource}/*.*")

    # 禁用部分文件的误编译
    set(SpecialFiles ${ProjectFiles})
    list(FILTER SpecialFiles INCLUDE REGEX ".*\.(hlsl|obj)")
    set_property(SOURCE ${SpecialFiles} PROPERTY VS_SETTINGS "ExcludedFromBuild=true")

    # 创建vs筛选器
    source_group(TREE "${ProjectSource}" FILES "${ProjectFiles}")
endmacro()

# 设置vs属性
macro(setVS)
    # 设置项目所在的解决方案目录
    cmake_path(GET ProjectSource PARENT_PATH ParentPath)
    parseRelativeDir(${ParentPath} relative_path) # 使用相对路径分类
    set(SolutionFolder "LightSources/${relative_path}")
    set_target_properties(${ProjectName} PROPERTIES FOLDER ${SolutionFolder})

    # 设置工作目录
    set_property(TARGET ${ProjectName} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endmacro()

# 设置项目为库
macro(setLibrary)
    add_library(${ProjectName} STATIC "${ProjectFiles}")
    setVS()
endmacro()

# 设置项目为可执行文件
macro(setExecutable)
    # 额外生成初始化文件，用于初始化依赖库
    set(ProjectInitFile "${ProjectSource}/__Init__.cpp")
    file(WRITE ${ProjectInitFile} "// ReSharper disable CppUnusedIncludeDirective\n")
    list(INSERT ProjectFiles 0 ${ProjectInitFile})

    add_executable(${ProjectName} "${ProjectFiles}")
    setVS()
endmacro()

# 设置项目为测试
macro(setTests)
    setExecutable()

    # 引用测试用的基本库（gtest和benchmark）
    find_package(GTest CONFIG REQUIRED)
    target_link_libraries(${ProjectName} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
    find_package(benchmark CONFIG REQUIRED)
    target_link_libraries(${ProjectName} PRIVATE benchmark::benchmark)

    # 引用测试目标库
    string(REPLACE "Tests" "" TargetLibrary ${ProjectName})
    linkLibrary(${TargetLibrary} ${ProjectName})
endmacro()

macro(addPackage)
    set(PackageSource ${CMAKE_CURRENT_SOURCE_DIR})
    set(PackageBinary ${CMAKE_CURRENT_BINARY_DIR})
    cmake_path(GET PackageSource FILENAME PackageName)
    message("添加包：${PackageName}")

    if(EXISTS "${PackageSource}/Runtime")
        addProject("Runtime")
        setLibrary()
    endif()

    if(EXISTS "${PackageSource}/Editor")
        addProject("Editor")
        setLibrary()
    endif()

    if(EXISTS "${PackageSource}/RuntimeTests")
        addProject("RuntimeTests")
        setTests()
    endif()

    if(EXISTS "${PackageSource}/EditorTests")
        addProject("EditorTests")
        setTests()
    endif()
endmacro()

# 链接目标库并在需要时自动保护库初始化文件
function(linkLibrary LibraryName ProjectName)
    target_link_libraries(${ProjectName} PRIVATE ${LibraryName})
    set(LibrarySource ${${LibraryName}Source}) # 获取库的源文件路径
    get_target_property(ProjectType ${ProjectName} TYPE) # 获取项目类型

    # 若项目为可执行项目，还需负责编译库的初始化文件
    if(${ProjectType} STREQUAL "EXECUTABLE")
        set(LibraryInitFile "${LibrarySource}/__Init__.h")

        if(EXISTS ${LibraryInitFile})
            file(APPEND ${ProjectInitFile} "#include \"${LibraryInitFile}\"\n")
        endif()
    endif()
endfunction()

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