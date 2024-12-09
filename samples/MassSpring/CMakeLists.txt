addProject()

linkModule(LightWindow)
linkModule(LightUtility)
linkModule(LightGraphics)
linkModule(LightMath)
linkModule(LightECS)
linkModule(LightUI)
linkModule(LightReflection)

find_package(magic_enum CONFIG REQUIRED)
target_link_libraries("${ProjectName}" PRIVATE magic_enum::magic_enum)