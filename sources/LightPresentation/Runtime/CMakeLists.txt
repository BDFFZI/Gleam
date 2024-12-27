addModule()

linkModule(LightWindow)
linkModule(LightUtility)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${ProjectName} PRIVATE glfw)

find_package(Vulkan REQUIRED)
target_link_libraries(${ProjectName} PRIVATE Vulkan::Vulkan)