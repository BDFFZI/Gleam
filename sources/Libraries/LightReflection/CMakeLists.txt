addPackage()

linkLibrary(LightUtilityRuntime "${PackageName}Runtime")

find_package(stduuid CONFIG REQUIRED)
target_link_libraries("${PackageName}Runtime" PUBLIC stduuid)

find_package(rapidjson CONFIG REQUIRED)
target_link_libraries("${PackageName}Runtime" PUBLIC rapidjson)