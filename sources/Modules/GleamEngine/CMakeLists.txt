addPackage()

linkLibrary(GleamECSRuntime ${PackageName}Runtime)
linkLibrary(GleamUtilityRuntime ${PackageName}Runtime)
linkLibrary(GleamReflectionRuntime ${PackageName}Runtime)
linkLibrary(GleamMathRuntime ${PackageName}Runtime)

linkLibrary(GleamUIRuntime ${PackageName}Editor)
linkLibrary(GleamReflectionRuntime ${PackageName}Editor)