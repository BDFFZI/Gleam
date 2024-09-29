#pragma once
#include <memory>

#include "Foundation/GLCommandPool.h"
#include "Foundation/GLDevice.h"
#include "Foundation/GLInstance.h"

class GL
{
public:
    static void Initialize(GLFWwindow* window);

    inline static std::unique_ptr<GLInstance> glInstance = {};
    inline static std::unique_ptr<GLSurface> glSurface = {};
    inline static std::unique_ptr<GLDevice> glDevice = {};
    inline static std::unique_ptr<GLCommandPool> glCommandPool = {};
};
