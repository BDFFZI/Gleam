addModule()

linkModule(LightWindow)
linkModule(LightUtility)

find_package(Vulkan REQUIRED)
target_link_libraries(${ProjectName} PUBLIC Vulkan::Vulkan)