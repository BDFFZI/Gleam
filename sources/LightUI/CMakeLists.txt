addModule()

target_link_libraries("${ModuleName}" PUBLIC LightGraphics)

target_link_libraries("${ModuleName}" PUBLIC LightWindow)

find_package(imgui CONFIG REQUIRED)
target_link_libraries("${ModuleName}" PUBLIC imgui::imgui)