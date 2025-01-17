addPackage()

linkLibrary(LightImportRuntime "${PackageName}RuntimeTests")
linkLibrary(LightUtilityRuntime "${PackageName}RuntimeTests")

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries("${PackageName}Runtime" PUBLIC glfw)

find_package(Vulkan REQUIRED)
target_link_libraries("${PackageName}Runtime" PUBLIC Vulkan::Vulkan)