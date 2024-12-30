addProject()

linkLibrary(LightEngineRuntime)
linkLibrary(LightMathRuntime)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries("${ProjectName}" PRIVATE glfw)