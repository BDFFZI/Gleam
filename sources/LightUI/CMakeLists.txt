addModule()

target_link_libraries("${ModuleName}" PRIVATE LightGraphics)

target_link_libraries("${ModuleName}" PRIVATE LightWindow)

target_link_libraries("${ModuleName}" PRIVATE LightMath)

find_package(imgui CONFIG REQUIRED)
target_link_libraries("${ModuleName}" PRIVATE imgui::imgui)
