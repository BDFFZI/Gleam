addPackage()

linkLibrary(LightEngineRuntime ${PackageName}Runtime)
linkLibrary(LightMathRuntime ${PackageName}Runtime)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries("${PackageName}Runtime" PUBLIC glfw)