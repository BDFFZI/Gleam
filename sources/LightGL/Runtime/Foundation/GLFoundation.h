#pragma once
#include <memory>

#include "GLCommandPool.h"
#include "GLDevice.h"
#include "GLInstance.h"

class GLFoundation
{
public:
    static void Initialize(GLFWwindow* window);

    inline static std::unique_ptr<GLInstance> glInstance = {};
    inline static std::unique_ptr<GLSurface> glSurface = {};
    inline static std::unique_ptr<GLDevice> glDevice = {};
    inline static std::unique_ptr<GLCommandPool> glCommandPool = {};
};
