project(MassSpring)

add_executable(MassSpring main.cpp)

target_link_libraries(MassSpring PUBLIC LightWindow)
target_link_libraries(MassSpring PUBLIC LightUtility)
target_link_libraries(MassSpring PUBLIC LightGraphics)
target_link_libraries(MassSpring PUBLIC LightMath)
target_link_libraries(MassSpring PUBLIC LightECS)