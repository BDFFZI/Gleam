#pragma once
#include <vulkan/vulkan_core.h>
#include "LightGL/Runtime/Pipeline/GLPipeline.h"

namespace Light
{
    struct GraphicsPreset
    {
        //帧缓冲区布局
        VkFormat colorFormat = VK_FORMAT_B8G8R8A8_SRGB;
        VkFormat depthStencilFormat = VK_FORMAT_D24_UNORM_S8_UINT;
        //网格布局
        GLMeshLayout meshLayout = {GLVertexInput(0, {}), GLInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false)};
        //着色器状态布局
        GLStateLayout stateLayout = []
        {
            GLStateLayout layout;
            //实现动态窗口大小
            layout.dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
            layout.dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
            layout.SetViewportCount(1, 1);
            return layout;
        }();

        GraphicsPreset() = default;
        GraphicsPreset(
            GLMeshLayout meshLayout)
            : meshLayout(std::move(meshLayout))
        {
        }
    };
}