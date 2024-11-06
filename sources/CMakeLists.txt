macro(addModule)
    cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME ModuleName)
    message("添加模块：${ModuleName}")

    project(${ModuleName})
    file(GLOB_RECURSE ALL_FILE "README.md"
        "Editor/**.cpp" "Editor/**.h" "Editor/**.hpp"
        "Runtime/**.cpp" "Runtime/**.h" "Runtime/**.hpp")
    add_library(${ModuleName} STATIC "${ALL_FILE}")

    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES "${ALL_FILE}")

endmacro()

macro(addTests)
    cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH TargetModulePath)
    cmake_path(GET TargetModulePath FILENAME TargetModuleName)
    set(ModuleName "${TargetModuleName}Tests")
    message("添加测试：${ModuleName}")

    project(${ModuleName})
    file(GLOB_RECURSE ALL_FILE "README.md" "*.cpp" "*.h")
    add_executable(${ModuleName} "${ALL_FILE}")

    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES "${ALL_FILE}")

    target_link_libraries(${ModuleName} PRIVATE ${TargetModuleName})

    find_package(GTest CONFIG REQUIRED)
    target_link_libraries(${ModuleName} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
    
    find_package(benchmark CONFIG REQUIRED)
    target_link_libraries(${ModuleName} PRIVATE benchmark::benchmark)

endmacro()

file(GLOB ModulePaths "*")
list(REMOVE_ITEM ModulePaths "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")

foreach(ModulePath ${ModulePaths})
    cmake_path(GET ModulePath FILENAME ModuleName)

    add_subdirectory("${ModulePath}")
    set_target_properties(${ModuleName} PROPERTIES FOLDER LightModules)

    if(EXISTS "${ModulePath}/Tests")
        add_subdirectory("${ModulePath}/Tests")
        set_target_properties("${ModuleName}Tests" PROPERTIES FOLDER LightTests)
    endif()

endforeach()