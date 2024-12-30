addProject()

linkLibrary(LightWindow)
linkLibrary(LightUtility)
linkLibrary(LightGraphics)
linkLibrary(LightMath)
linkLibrary(LightECS)
linkLibrary(LightUI)
linkLibrary(LightReflection)

find_package(magic_enum CONFIG REQUIRED)
target_link_libraries("${ProjectName}" PRIVATE magic_enum::magic_enum)