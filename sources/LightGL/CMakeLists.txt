addModule()

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${ModuleName} PRIVATE glfw)

find_package(Vulkan REQUIRED)
target_link_libraries(${ModuleName} PRIVATE Vulkan::Vulkan)