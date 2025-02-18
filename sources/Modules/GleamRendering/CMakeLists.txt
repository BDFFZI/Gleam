addPackage()

linkLibrary(GleamPresentationRuntime ${PackageName}Runtime)
linkLibrary(GleamMathRuntime ${PackageName}Runtime)

find_package(imguizmo CONFIG REQUIRED)
target_link_libraries(${PackageName}Editor PUBLIC imguizmo::imguizmo)