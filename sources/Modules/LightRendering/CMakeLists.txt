addPackage()

linkLibrary(LightPresentationRuntime ${PackageName}Runtime)
linkLibrary(LightMathRuntime ${PackageName}Runtime)

linkLibrary(LightEngineEditor ${PackageName}RuntimeTests)

find_package(imguizmo CONFIG REQUIRED)
target_link_libraries(${PackageName}Editor PUBLIC imguizmo::imguizmo)