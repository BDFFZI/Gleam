addModule()

target_link_libraries("${ModuleName}" PUBLIC LightMath)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries("${ModuleName}" PUBLIC glfw)