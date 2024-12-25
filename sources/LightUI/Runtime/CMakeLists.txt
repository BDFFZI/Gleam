addModule()

linkModule(LightPresentation)

find_package(imgui CONFIG REQUIRED)
target_link_libraries("${ProjectName}" PRIVATE imgui::imgui)
