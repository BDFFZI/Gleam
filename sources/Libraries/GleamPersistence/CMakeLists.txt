addPackage()

linkLibrary(GleamReflectionRuntime ${PackageName}Runtime)

find_package(rapidjson CONFIG REQUIRED)
target_link_libraries("${PackageName}Runtime" PUBLIC rapidjson)