addPackage()

linkLibrary(GleamEngineRuntime ${PackageName}Runtime)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries("${PackageName}Runtime" PUBLIC glfw)