addPackage()

linkLibrary(LightECSRuntime ${PackageName}Runtime)
linkLibrary(LightUtilityRuntime ${PackageName}Runtime)
linkLibrary(LightReflectionRuntime ${PackageName}Runtime)
linkLibrary(LightMathRuntime ${PackageName}Runtime)

linkLibrary(LightUIRuntime ${PackageName}Editor)
linkLibrary(LightReflectionRuntime ${PackageName}Editor)