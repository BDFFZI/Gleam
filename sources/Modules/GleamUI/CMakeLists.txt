addPackage()

linkLibrary(GleamPresentationRuntime ${PackageName}Runtime)

# 必须设置为PUBLIC，否则会覆盖其他库的导入设置，导致全部都为PRIVATE而缺少头文件引用（imgui导入配置的bug？）
find_package(imgui CONFIG REQUIRED)
target_link_libraries("${PackageName}Runtime" PUBLIC imgui::imgui)

find_package(magic_enum CONFIG REQUIRED)
target_link_libraries("${PackageName}Runtime" PUBLIC magic_enum::magic_enum)