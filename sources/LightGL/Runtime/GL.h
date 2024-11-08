#pragma once
#include <memory>

#include "Foundation/GLCommandPool.h"
#include "Foundation/GLDevice.h"
#include "Foundation/GLInstance.h"

class GL
{
public:
    static GL Initialize(GLFWwindow* window,const std::vector<const char*>& extensions = {}, void* extensionFeatures = VK_NULL_HANDLE);
    static void UnInitialize();

    inline static GLFWwindow* glfwWindow = nullptr;
    inline static std::unique_ptr<GLInstance> glInstance = {};
    inline static std::unique_ptr<GLSurface> glSurface = {};
    inline static std::unique_ptr<GLDevice> glDevice = {};
    inline static std::unique_ptr<GLCommandPool> glCommandPool = {};

private:
    GL() = default;
};
