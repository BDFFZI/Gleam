addModule()

target_link_libraries("${ModuleName}" PRIVATE LightMath)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries("${ModuleName}" PRIVATE glfw)