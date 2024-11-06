macro(addProject)
    cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME ProjectName)

    project(${ProjectName})
    file(GLOB_RECURSE ALL_FILE "README.md"
        "*.h" "*.cpp" "*.hpp")
    add_executable(${ProjectName} ${ALL_FILE})

    set_target_properties(${ProjectName} PROPERTIES FOLDER LightSamples)
endmacro()

macro(linkModule ModuleName)
    target_link_libraries(MassSpring PUBLIC ${ModuleName})
    if(EXISTS "${CMAKE_SOURCE_DIR}/sources/${ModuleName}/Assets")
        file(COPY "${CMAKE_SOURCE_DIR}/sources/${ModuleName}/Assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/../${ProjectName}")
    endif()
endmacro()

file(GLOB ProjectPaths "*")
list(REMOVE_ITEM ProjectPaths "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")

foreach(ProjectPath ${ProjectPaths})
    cmake_path(GET ProjectPath FILENAME ProjectName)
    add_subdirectory(${ProjectPath})
endforeach()