addModule()

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${ProjectName} PUBLIC glfw)

find_package(Vulkan REQUIRED)
target_link_libraries(${ProjectName} PUBLIC Vulkan::Vulkan)