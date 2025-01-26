#pragma once
#include "LightGL/Runtime/Pipeline/GLMeshLayout.h"
#include "LightGL/Runtime/Resource/GLDescriptorSetLayout.h"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightMath/Runtime/Matrix.h"
#include "LightMath/Runtime/Vector.h"

namespace Light
{
    //网格布局
    struct Vertex
    {
        float3 position;
        float4 color;
        //纹理采样
        float2 uv;
        //构建法线空间
        float3 normal;
        float3 tangent;
    };

    struct PushConstant
    {
        alignas(16) float4x4 MatrixMVP;
    };

    class RenderingConfig
    {
    public:
        //着色器输入输出布局
        inline static VkFormat ColorFormat = VK_FORMAT_B8G8R8A8_SRGB;
        inline static VkFormat DepthStencilFormat = VK_FORMAT_D24_UNORM_S8_UINT;
        inline static VkSampleCountFlagBits SampleCount = VK_SAMPLE_COUNT_1_BIT;
        inline static const GLVertexInput VertexInput = {
            sizeof(Vertex), {
                GLVertexAttribute{0,offsetof(Vertex, position), VK_FORMAT_R32G32B32_SFLOAT},
                GLVertexAttribute{1,offsetof(Vertex, color), VK_FORMAT_R32G32B32A32_SFLOAT},
                GLVertexAttribute{2,offsetof(Vertex, uv), VK_FORMAT_R32G32_SFLOAT},
                GLVertexAttribute{3,offsetof(Vertex, normal), VK_FORMAT_R32G32B32_SFLOAT},
                GLVertexAttribute{4,offsetof(Vertex, tangent), VK_FORMAT_R32G32B32_SFLOAT},
            }
        };
        inline static const GLMeshLayout TriangleMeshLayout = {
            VertexInput, {VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false}
        };
        inline static const GLMeshLayout LineMeshLayout = {
            VertexInput, {VK_PRIMITIVE_TOPOLOGY_LINE_LIST, false}
        };
        inline static const GLMeshLayout PointMeshLayout = {
            VertexInput, {VK_PRIMITIVE_TOPOLOGY_POINT_LIST, false}
        };
        //着色器资源布局
        inline static const std::vector<GLDescriptorBinding> DescriptorBindings = {
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}, //AlbedoTex（rgb：颜色，a：透明度）
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}, //MetallicGlossTex（rgb：金属度，a：光泽度）
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}, //NormalTex（rgb：法线）
        };
        inline static const std::vector<VkPushConstantRange> PushConstantRanges = {
            {VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstant)} //MatrixMVP
        };

        static std::unique_ptr<GraphicsConfig> CreateGraphicsConfig()
        {
            std::unique_ptr<GraphicsConfig> graphicsConfig = std::make_unique<GraphicsConfig>();
            graphicsConfig->presentColorFormat = ColorFormat;
            graphicsConfig->presentDepthStencilFormat = DepthStencilFormat;
            graphicsConfig->presentSampleCount = SampleCount;
            graphicsConfig->defaultGSAssetLayout = GSAssetLayout(
                DescriptorBindings,
                PushConstantRanges
            );
            graphicsConfig->defaultGSInoutLayout = GSInoutLayout(
                TriangleMeshLayout,
                graphicsConfig->presentColorFormat,
                graphicsConfig->presentDepthStencilFormat
            );
            return std::move(graphicsConfig);
        }
    };
}