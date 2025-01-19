addProject()

linkLibrary(LightWindowRuntime ${ProjectName})
linkLibrary(LightUtilityRuntime ${ProjectName})
linkLibrary(LightGraphicsRuntime ${ProjectName})
linkLibrary(LightMathRuntime ${ProjectName})
linkLibrary(LightECSRuntime ${ProjectName})
linkLibrary(LightUIRuntime ${ProjectName})
linkLibrary(LightReflectionRuntime ${ProjectName})

find_package(magic_enum CONFIG REQUIRED)
target_link_libraries(${ProjectName} PRIVATE magic_enum::magic_enum)