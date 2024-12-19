macro(addModule)
    # 获取模块名（即文件夹名）
    cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME ModuleName)
    message("添加模块：${ModuleName}")

    # 获取文件夹内所有文件，除了测试文件夹
    file(GLOB_RECURSE ALL_FILE "*.*")
    list(FILTER ALL_FILE EXCLUDE REGEX "Tests/.*")

    # 创建项目
    project(${ModuleName})
    add_library(${ModuleName} STATIC "${ALL_FILE}")

    # 创建vs筛选器
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES "${ALL_FILE}")
endmacro()

macro(addTests)
    # 获取测试名（模块名+Tests）
    cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH TargetModulePath)
    cmake_path(GET TargetModulePath FILENAME TargetModuleName)
    set(ModuleName "${TargetModuleName}Tests")
    message("添加测试：${ModuleName}")

    # 获取文件夹内所有文件
    file(GLOB_RECURSE ALL_FILE "*.*")

    # 创建项目
    project(${ModuleName})
    add_executable(${ModuleName} "${ALL_FILE}")

    # 创建vs筛选器
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES "${ALL_FILE}")

    # 引用测试的目标模块
    target_link_libraries(${ModuleName} PRIVATE ${TargetModuleName})
    # 引用测试用的基本库（gtest和benchmark）
    find_package(GTest CONFIG REQUIRED)
    target_link_libraries(${ModuleName} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
    find_package(benchmark CONFIG REQUIRED)
    target_link_libraries(${ModuleName} PRIVATE benchmark::benchmark)
endmacro()

# 递归添加子目录的函数
function(add_subdirectories_recursive current_dir)
    file(GLOB children RELATIVE ${current_dir} ${current_dir}/*) # 获取当前目录下的所有文件和文件夹的相对路径

    foreach(child ${children}) # 遍历所有目标
        # 跳过非文件夹
        if(NOT IS_DIRECTORY ${current_dir}/${child})
            continue()
        endif()

        # 如果文件夹包含CMake文件则是项目
        if(EXISTS ${current_dir}/${child}/CMakeLists.txt)
            add_subdirectory(${current_dir}/${child}) # 添加该子项目

            # 分类到解决方案文件夹
            file(RELATIVE_PATH relative_path ${CMAKE_CURRENT_SOURCE_DIR} ${current_dir}) # 使用相对路径分类
            set_target_properties(${child} PROPERTIES FOLDER "LightModules/${relative_path}")

            # 判断是否有测试文件夹，有则添加测试项目
            if(EXISTS "${child}/Tests")
                add_subdirectory("${current_dir}/${child}/Tests")
                set_target_properties("${child}Tests" PROPERTIES FOLDER "LightTests")
            endif()
            continue()
        endif()

        # 对于其他文件夹需递归检查
        add_subdirectories_recursive(${current_dir}/${child})
    endforeach()
endfunction()

# 从当前目录开始递归添加子目录
add_subdirectories_recursive(${CMAKE_CURRENT_SOURCE_DIR})