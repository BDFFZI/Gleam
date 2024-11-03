#pragma once
#include <memory>

#include "Foundation/GLCommandPool.h"
#include "Foundation/GLDevice.h"
#include "Foundation/GLInstance.h"

class GL
{
public:
    static GL Initialize(GLFWwindow* window);
    static void UnInitialize();

    inline static std::unique_ptr<GLInstance> glInstance = {};
    inline static std::unique_ptr<GLSurface> glSurface = {};
    inline static std::unique_ptr<GLDevice> glDevice = {};
    inline static std::unique_ptr<GLCommandPool> glCommandPool = {};

private:
    GL() = default;
};
