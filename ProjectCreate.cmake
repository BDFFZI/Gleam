# 计算包基本信息
macro(initPackageInfo)
    set(PackageSource ${CMAKE_CURRENT_SOURCE_DIR})
    set(PackageBinary ${CMAKE_CURRENT_BINARY_DIR})
    cmake_path(GET PackageSource FILENAME PackageName)
    message("创建包：${PackageName}")
endmacro()

# 计算包项目基本信息
macro(initPackageProjectInfo projectSubPath)
    set(ProjectSource "${PackageSource}/${projectSubPath}")
    set(ProjectBinary "${PackageBinary}/${projectSubPath}")
    set(ProjectName "${PackageName}${projectSubPath}")
    message("创建包项目：${ProjectName}")
endmacro()

# 计算项目基本信息
macro(initProjectInfo)
    set(ProjectSource ${CMAKE_CURRENT_SOURCE_DIR})
    set(ProjectBinary ${CMAKE_CURRENT_BINARY_DIR})
    cmake_path(GET ProjectSource FILENAME ProjectName)
    message("创建项目：${ProjectName}")
endmacro()

# 根据项目基本信息创建项目
macro(initProject)
    project(${ProjectName})

    # 存储项目关键信息
    set(${ProjectName}Source ${ProjectSource} CACHE STRING "项目的SOURCE目录" FORCE)
    set(${ProjectName}Binary ${ProjectBinary} CACHE STRING "项目的Binary目录" FORCE)
    set(${ProjectName}Dependencies "" CACHE STRING "项目依赖的其他项目" FORCE)
    set(${ProjectName}Resources "" CACHE STRING "项目依赖的资源" FORCE)

    # 统计所有项目
    list(APPEND AllProjects ${ProjectName})
    set(AllProjects ${AllProjects} CACHE STRING "全部项目" FORCE)

    # 获取项目文件
    file(GLOB_RECURSE ProjectFiles "${ProjectSource}/*.*")
    set(SpecialFiles ${ProjectFiles}) # 取消对部分项目文件的误编译
    list(FILTER SpecialFiles INCLUDE REGEX ".*\.(hlsl|obj)")
    set_property(SOURCE ${SpecialFiles} PROPERTY VS_SETTINGS "ExcludedFromBuild=true")
endmacro()

# 设置vs属性（vs属性只能在设置项目类型后才可设置，所以不能在创建项目时设置）
macro(setVS)
    # 设置项目所在的解决方案分类
    cmake_path(GET ProjectSource PARENT_PATH ParentPath)
    file(RELATIVE_PATH SolutionFolder ${CMAKE_SOURCE_DIR} ${ParentPath}) # 根据父目录相对根目录的路径分类
    set_target_properties(${ProjectName} PROPERTIES FOLDER ${SolutionFolder})

    # 设置输出目录和工作目录
    set_target_properties(${ProjectName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${ProjectName}")
    set_target_properties(${ProjectName} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${ProjectName}")

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
    # 生成初始化文件，该文件将被用于编译库初始化文件，从而实现跨库的全局变量初始化（否则部分代码会因优化而剥离，导致无法利用全局变量初始化执行事件）
    set(ProjectInitFile "${ProjectSource}/__Init__.cpp")
    file(WRITE ${ProjectInitFile} "// ReSharper disable CppUnusedIncludeDirective\n")
    list(INSERT ProjectFiles 0 ${ProjectInitFile}) # 将初始化文件添加到项目文件中

    add_executable(${ProjectName} "${ProjectFiles}")
    setVS()
endmacro()

# 设置项目为测试
macro(setTests)
    setExecutable()

    # 引用测试工具库（gtest和benchmark）
    find_package(GTest CONFIG REQUIRED)
    target_link_libraries(${ProjectName} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
    find_package(benchmark CONFIG REQUIRED)
    target_link_libraries(${ProjectName} PRIVATE benchmark::benchmark)

    # 引用被测试库
    string(REPLACE "Tests" "" TargetLibrary ${ProjectName})
    linkLibrary(${TargetLibrary} ${ProjectName})
endmacro()

macro(addPackage)
    initPackageInfo()

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

        # 引用对应的运行时库
        string(REPLACE "Editor" "Runtime" TargetLibrary ${ProjectName})
        linkLibrary(${TargetLibrary} ${ProjectName})

        # 引用编辑器核心库
        if(NOT ${PackageName} STREQUAL "GleamEngine")
            linkLibrary("GleamEngineEditor" ${ProjectName})
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
    list(APPEND ${ProjectName}Dependencies ${LibraryName})
    set(${ProjectName}Dependencies ${${ProjectName}Dependencies} CACHE STRING "项目依赖的其他项目" FORCE)

    # 若依赖项目未添加到全部项目，则插入到项目前一位，以便后续按依赖顺序处理项目
    if(NOT ${LibraryName} IN_LIST AllProjects)
        list(FIND AllProjects ${ProjectName} Index)
        list(INSERT AllProjects ${Index} ${LibraryName})
        set(AllProjects ${AllProjects} CACHE STRING "全部项目" FORCE)
    endif()
endmacro()