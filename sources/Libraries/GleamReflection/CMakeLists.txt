addPackage()

linkLibrary(GleamUtilityRuntime "${PackageName}Runtime")

find_package(stduuid CONFIG REQUIRED)
target_link_libraries("${PackageName}Runtime" PUBLIC stduuid)
target_compile_definitions("${PackageName}Runtime" PUBLIC UUID_SYSTEM_GENERATOR)

find_package(rapidjson CONFIG REQUIRED)
target_link_libraries("${PackageName}Runtime" PUBLIC rapidjson)