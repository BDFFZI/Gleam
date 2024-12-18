addModule()

target_link_libraries("${ModuleName}" PUBLIC LightWindow)
target_link_libraries("${ModuleName}" PUBLIC LightGL)
target_link_libraries("${ModuleName}" PUBLIC LightGraphics)
target_link_libraries("${ModuleName}" PUBLIC LightMath)
target_link_libraries("${ModuleName}" PUBLIC LightECS)
target_link_libraries("${ModuleName}" PUBLIC LightUI)