#pragma once
#include <GLFW/glfw3.h>

// ReSharper disable CppUnusedIncludeDirective
#include "Foundation/GLSurface.h"
#include "Foundation/GLFoundation.h"
#include "Pipeline/GLFramebuffer.h"
#include "Pipeline/GLPipeline.h"
#include "Pipeline/GLRenderPass.h"
#include "Resource/GLDescriptorPool.h"
#include "Resource/GLImageSampler.h"
#include "Resource/GLBuffer.h"
#include "Pipeline/GLCommandBuffer.h"
#include "Pipeline/GLSwapChain.h"
// ReSharper restore CppUnusedIncludeDirective

class GL
{
public:
    static void Initialize(GLFWwindow* window);
};
