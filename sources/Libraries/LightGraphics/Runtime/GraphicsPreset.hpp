#pragma once
#include <vulkan/vulkan_core.h>
#include "GraphicsAssets.h"
#include "LightGL/Runtime/Pipeline/GLPipeline.h"
#include "LightMath/Runtime/Matrix.hpp"

namespace Light
{
    struct PushConstant
    {
        alignas(16) float4x4 MatrixMVP;
    };

    inline static const std::vector<VkPushConstantRange> PushConstantRanges = {
        {VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstant)} //快速MVP矩阵变换
    };

    struct GraphicsPreset
    {
        //帧缓冲区布局
        VkFormat colorFormat = VK_FORMAT_B8G8R8A8_SRGB;
        VkFormat depthStencilFormat = VK_FORMAT_D24_UNORM_S8_UINT;
        //着色器布局
        GLStateLayout stateLayout = []
        {
            GLStateLayout layout;
            //实现动态窗口大小
            layout.dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
            layout.dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
            layout.SetViewportCount(1, 1);
            return layout;
        }();
        std::vector<GLDescriptorBinding> descriptorBindings = {};
        //网格布局
        GLMeshLayout meshLayout = {};

        GraphicsPreset() = default;
        GraphicsPreset(const GLMeshLayout& meshLayout)
            : meshLayout(meshLayout)
        {
        }
    };
}