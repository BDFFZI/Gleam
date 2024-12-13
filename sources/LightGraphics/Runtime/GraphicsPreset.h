#pragma once
#include <vulkan/vulkan_core.h>

#include "LightGL/Runtime/Pipeline/GLPipeline.h"
#include "LightGL/Runtime/Resource/GLImageSampler.h"
#include "LightMath/Runtime/Matrix.hpp"


namespace Light
{
    struct GraphicsPreset
    {
        struct PushConstant
        {
            alignas(16) float4x4 MatrixMVP;
        };

        struct Vertex
        {
            float3 position;
            //构建法线空间
            float3 normal;
            float3 tangent;
            //纹理采样
            float2 uv;
        };

        //帧缓冲区布局
        static constexpr VkFormat DefaultColorFormat = VK_FORMAT_B8G8R8A8_SRGB;
        static constexpr VkFormat DefaultDepthStencilFormat = VK_FORMAT_D24_UNORM_S8_UINT;
        //网格布局
        inline static const GLVertexInput DefaultVertexInput = {
            sizeof(Vertex), {
                GLVertexAttribute{0,offsetof(Vertex, position), VK_FORMAT_R32G32B32_SFLOAT},
                GLVertexAttribute{1,offsetof(Vertex, normal), VK_FORMAT_R32G32B32_SFLOAT},
                GLVertexAttribute{2,offsetof(Vertex, tangent), VK_FORMAT_R32G32B32_SFLOAT},
                GLVertexAttribute{3,offsetof(Vertex, uv), VK_FORMAT_R32G32_SFLOAT},
            }
        };
        inline static const GLInputAssembly DefaultInputAssembly = {
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            false
        };
        inline static const GLMeshLayout DefaultMeshLayout = {
            DefaultVertexInput, DefaultInputAssembly
        };
        //着色器布局
        inline static const GLStateLayout DefaultStateLayout = []
        {
            GLStateLayout layout;
            //实现动态窗口大小
            layout.dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
            layout.dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
            layout.SetViewportCount(1, 1);
            return layout;
        }();
        inline static const std::vector<GLDescriptorBinding> DefaultDescriptorBindings = {
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}, //反照率与透明度贴图
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}, //金属度与光泽度贴图
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT} //法线贴图
        };
        inline static const std::vector<VkPushConstantRange> DefaultPushConstantRanges = {
            {VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstant)} //快速MVP矩阵变换
        };
        //运行时GL对象
        inline static std::unique_ptr<GLDescriptorSetLayout> DefaultGLDescriptorSetLayout = {};
        inline static std::unique_ptr<GLPipelineLayout> DefaultGLPipelineLayout = {};
        inline static std::unique_ptr<GLImageSampler> DefaultGLImageSampler = {};

    private:
        friend class Graphics;
        static void Initialize();
        static void UnInitialize();
    };
}
