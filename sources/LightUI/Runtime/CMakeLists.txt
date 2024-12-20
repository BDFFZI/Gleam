addModule()

target_link_libraries("${ProjectName}" PRIVATE LightGraphics)

target_link_libraries("${ProjectName}" PRIVATE LightWindow)

target_link_libraries("${ProjectName}" PRIVATE LightMath)

find_package(imgui CONFIG REQUIRED)
target_link_libraries("${ProjectName}" PRIVATE imgui::imgui)
