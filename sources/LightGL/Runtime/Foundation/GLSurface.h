#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "GLInstance.h"

/**
 * VkSurfaceKHR相当于是vulkan中的窗口对象
 */
class GLSurface
{
public:
    VkSurfaceKHR surface;
    GLFWwindow* window;

    GLSurface(const GLInstance& glInstance, GLFWwindow* window);
    GLSurface(const GLSurface&) = delete;
    ~GLSurface();
private:
    VkInstance instance;
};