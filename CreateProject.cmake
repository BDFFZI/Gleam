function(parseRelativeDir in_path out_dir)
    file(RELATIVE_PATH relative_path ${CMAKE_SOURCE_DIR} ${in_path})
    set(${out_dir} ${relative_path} PARENT_SCOPE)
endfunction()

function(parseName in_path out_projectName)
    parseRelativeDir(${in_path} relative_path)
    string(REPLACE "/" "" projectName ${relative_path})
    set(${out_projectName} ${projectName} PARENT_SCOPE)
endfunction()

macro(initPackageProjectInfo projectSubPath)
    # 获取路径和名称信息
    set(ProjectSource "${PackageSource}/${projectSubPath}")
    set(ProjectBinary "${PackageBinary}/${projectSubPath}")
    set(ProjectName "${PackageName}${projectSubPath}")
endmacro()

macro(initProjectInfo)
    # 获取路径和名称信息
    set(ProjectSource ${CMAKE_CURRENT_SOURCE_DIR})
    set(ProjectBinary ${CMAKE_CURRENT_BINARY_DIR})
    cmake_path(GET ProjectSource FILENAME ProjectName)
endmacro()

macro(initProject)
    # 存储项目信息
    set(${ProjectName} TRUE CACHE BOOL "项目是否存在" FORCE)
    set(${ProjectName}Source ${ProjectSource} CACHE STRING "项目的SOURCE目录" FORCE)
    set(${ProjectName}Binary ${ProjectBinary} CACHE STRING "项目的Binary目录" FORCE)
    set(${ProjectName}Dependencies "" CACHE STRING "项目依赖的其他项目" FORCE)

    # 创建项目
    project(${ProjectName})
    message("添加项目：${ProjectName}")

    # 获取项目文件
    file(GLOB_RECURSE ProjectFiles "${ProjectSource}/*.*")

    # 禁用部分文件的误编译
    set(SpecialFiles ${ProjectFiles})
    list(FILTER SpecialFiles INCLUDE REGEX ".*\.(hlsl|obj)")
    set_property(SOURCE ${SpecialFiles} PROPERTY VS_SETTINGS "ExcludedFromBuild=true")
endmacro()

# 设置vs属性
macro(setVS)
    # 设置项目所在的解决方案目录
    cmake_path(GET ProjectSource PARENT_PATH ParentPath)
    parseRelativeDir(${ParentPath} relative_path) # 使用相对路径分类
    set(SolutionFolder ${relative_path})
    set_target_properties(${ProjectName} PROPERTIES FOLDER ${SolutionFolder})

    # 设置工作目录
    set_property(TARGET ${ProjectName} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

    # 创建vs筛选器
    source_group(TREE "${ProjectSource}" FILES "${ProjectFiles}")
endmacro()

# 设置项目为库
macro(setLibrary)
    add_library(${ProjectName} STATIC "${ProjectFiles}")
    setVS()

    # 添加以自身命名的具有传染性质的宏，以便让其他项目可以在代码中判断库链接情况
    target_compile_definitions(${ProjectName} PUBLIC ${ProjectName})
endmacro()

# 设置项目为可执行文件
macro(setExecutable)
    # 生成初始化文件，该文件将被显式编译，用于实现跨库的全局变量初始化（否则部分代码会因优化而剥离，导致无法利用全局变量初始化执行事件）
    set(ProjectInitFile "${ProjectSource}/__Init__.cpp")
    file(WRITE ${ProjectInitFile} "// ReSharper disable CppUnusedIncludeDirective\n")
    list(INSERT ProjectFiles 0 ${ProjectInitFile})

    add_executable(${ProjectName} "${ProjectFiles}")
    setVS()

    # 统计所有可执行项目
    list(APPEND AllExecutable ${ProjectName})
    set(AllExecutable ${AllExecutable} CACHE STRING "全部可执行项目" FORCE)
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
        initPackageProjectInfo("Runtime")
        initProject()

        if(EXISTS "${PackageSource}/ReadMe.md")
            list(APPEND ProjectFiles "${PackageSource}/ReadMe.md")
        endif()

        setLibrary()
    endif()

    if(EXISTS "${PackageSource}/Editor")
        initPackageProjectInfo("Editor")
        initProject()
        setLibrary()

        string(REPLACE "Editor" "Runtime" TargetLibrary ${ProjectName})
        linkLibrary(${TargetLibrary} ${ProjectName})

        if(NOT ${PackageName} STREQUAL "LightEngine")
            linkLibrary("LightEngineEditor" ${ProjectName})
        endif()
    endif()

    if(EXISTS "${PackageSource}/RuntimeTests")
        initPackageProjectInfo("RuntimeTests")
        initProject()
        setTests()
    endif()

    if(EXISTS "${PackageSource}/EditorTests")
        initPackageProjectInfo("EditorTests")
        initProject()
        setTests()
    endif()
endmacro()

macro(addProject)
    initProjectInfo()
    initProject()
    setExecutable()
endmacro()

# 链接目标库并自动创建库初始化文件
macro(linkLibrary LibraryName ProjectName)
    target_link_libraries(${ProjectName} PUBLIC ${LibraryName})

    # 添加依赖项信息
    list(APPEND ${ProjectName}Dependencies ${${LibraryName}Dependencies})
    list(APPEND ${ProjectName}Dependencies ${LibraryName})
    list(REMOVE_DUPLICATES ${ProjectName}Dependencies)
    set(${ProjectName}Dependencies ${${ProjectName}Dependencies} CACHE STRING "项目依赖的其他项目" FORCE)

    get_target_property(ProjectType ${ProjectName} TYPE) # 获取项目类型

    # 可执行项目作为最终编译目标需要进行一些处理
    if(${ProjectType} STREQUAL "EXECUTABLE")
        set(LibraryInitFile "${${LibraryName}Source}/__Init__.h") # 库初始化文件

        # 目标库存在初始化文件，需要记录到项目初始化文件中
        if(EXISTS ${LibraryInitFile})
            set(ProjectInitFile "${${ProjectName}Source}/__Init__.cpp") # 项目初始化地址
            file(RELATIVE_PATH relative_path "${${LibraryName}Source}/../.." ${LibraryInitFile})
            file(APPEND ${ProjectInitFile} "#include \"${relative_path}\"\n")
        endif()
    endif()
endmacro()