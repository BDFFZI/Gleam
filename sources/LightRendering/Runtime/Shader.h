#pragma once
#include <string>
#include <vector>

#include "LightGL/Runtime/Pipeline/GLPipeline.h"
#include "LightGL/Runtime/Resource/GLDescriptorSetLayout.h"
#include "Mesh.h"
#include "LightGraphics/Runtime/Shader.h"

namespace Light
{
    constexpr VkFormat DefaultColorFormat = VK_FORMAT_B8G8R8A8_SRGB;
    constexpr VkFormat DefaultDepthStencilFormat = VK_FORMAT_D24_UNORM_S8_UINT;
    inline static const GLStateLayout DefaultStateLayout = []
    {
        GLStateLayout layout;
        layout.dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
        layout.dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
        layout.SetViewportCount(1, 1);
        return layout;
    }();

    struct PushConstant
    {
        alignas(16) float4x4 MatrixMVP;
    };

    class Shader : public ShaderT
    {
    public:
        /**
         * @brief 通过文件加载符合规范的着色器
         *
         * 规范要求：
         * 1. 只支持顶点和片段着色器，且名称必须分别为@c VertexShader @c FragmentShader 。
         * 2. 只能使用内置的推送常量类型。
         * 3. 描述符集只能以推送的方式使用。
         * 4. 输出的帧缓冲区格式只能使用交换链中的格式。
         * 5. 默认使用内置的网格布局
         * 
         * @param shaderFile 
         * @param descriptorSetLayout 
         * @param stateLayout
         * @param colorFormat
         * @param depthStencilFormat
         * @return 
         */
        Shader(
            const std::string& shaderFile,
            const std::vector<GLDescriptorBinding>& descriptorSetLayout = {},
            const GLStateLayout& stateLayout = DefaultStateLayout,
            VkFormat colorFormat = DefaultColorFormat,
            VkFormat depthStencilFormat = DefaultDepthStencilFormat
        );
    };
}
