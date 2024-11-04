project(MassSpring)

file(GLOB_RECURSE ALL_FILE "README.md"
    "*.h" "*.cpp" "*.hpp")
add_executable(MassSpring ${ALL_FILE})

target_link_libraries(MassSpring PUBLIC LightWindow)
target_link_libraries(MassSpring PUBLIC LightUtility)
target_link_libraries(MassSpring PUBLIC LightGraphics)
target_link_libraries(MassSpring PUBLIC LightMath)
target_link_libraries(MassSpring PUBLIC LightECS)
target_link_libraries(MassSpring PUBLIC LightUI)