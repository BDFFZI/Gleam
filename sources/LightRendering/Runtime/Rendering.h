#pragma once
#include <vulkan/vulkan_core.h>

#include "LightGL/Runtime/Pipeline/GLPipeline.h"

class Rendering
{
public:

    struct PushConstant
    {
        alignas(16) float4x4 MatrixMVP;
    };

    static void InitializeGraphicsDemand(VkFormat& ColorFormat);
};
