addModule()

target_link_libraries("${ModuleName}" PUBLIC LightGraphics)

find_package(imgui CONFIG REQUIRED)
target_link_libraries("${ModuleName}" PUBLIC imgui::imgui)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries("${ModuleName}" PUBLIC glfw)
