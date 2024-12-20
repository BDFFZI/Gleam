addModule()

target_link_libraries("${ProjectName}" PRIVATE LightMath)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries("${ProjectName}" PRIVATE glfw)