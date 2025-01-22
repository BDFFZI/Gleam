addPackage()

linkLibrary(LightMathRuntime "${PackageName}Runtime")
linkLibrary(LightUtilityRuntime "${PackageName}RuntimeTests")

find_package(Stb REQUIRED)
target_include_directories("${PackageName}Runtime" PRIVATE ${Stb_INCLUDE_DIR})

find_package(tinyobjloader CONFIG REQUIRED)
target_link_libraries("${PackageName}Runtime" PRIVATE tinyobjloader::tinyobjloader)

find_package(unofficial-shaderc CONFIG REQUIRED)
target_link_libraries("${PackageName}Runtime" PRIVATE unofficial::shaderc::shaderc)