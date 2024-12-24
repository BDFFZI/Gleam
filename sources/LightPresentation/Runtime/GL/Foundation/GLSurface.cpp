#include "GLSurface.h"

#include <stdexcept>


GLSurface::GLSurface(const GLInstance& glInstance, GLFWwindow* window)
    : window(window), instance(glInstance.instance)
{
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
        throw std::runtime_error("创建窗口表面失败！");
}
GLSurface::~GLSurface()
{
    vkDestroySurfaceKHR(instance, surface, nullptr);
}
