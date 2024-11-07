#pragma once
#include "LightGL/Runtime/Pipeline/GLCommandBuffer.h"
#include "LightGL/Runtime/Pipeline/GLMeshLayout.h"
#include "LightGL/Runtime/Pipeline/GLPipeline.h"
#include "LightGL/Runtime/Resource/GLBuffer.h"
#include "LightMath/Runtime/Matrix.hpp"
#include "LightMath/Runtime/Vector.hpp"


namespace Light
{
    constexpr VkFormat BuiltInColorFormat = VK_FORMAT_B8G8R8A8_SRGB;
    constexpr VkFormat BuiltInDepthStencilFormat = VK_FORMAT_D24_UNORM_S8_UINT;

    struct BuiltInPushConstant
    {
        alignas(16) float4x4 MatrixMVP;
    };

    const GLStateLayout BuiltInGLStateLayout = []
    {
        GLStateLayout stateLayout = {};
        //解耦着色器与多重采样的关系，根据渲染目标属性实时调整
        // stateLayout.dynamicStates.push_back(VK_DYNAMIC_STATE_RASTERIZATION_SAMPLES_EXT);
        //解耦着色器与网格的关系，实现不同网格共用着色器
        // stateLayout.dynamicStates.push_back(VK_DYNAMIC_STATE_VERTEX_INPUT_EXT);
        // stateLayout.dynamicStates.push_back(VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY);
        // stateLayout.dynamicStates.push_back(VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE);

        return stateLayout;
    }();

    struct BuiltInVertex
    {
        float3 position;
        float3 normal;
        float4 tangent;
        float2 uv;
        float4 color;
    };

    const GLVertexInput BuiltInGLVertexInput = {
        sizeof(BuiltInVertex), {
            GLVertexAttribute{0,offsetof(BuiltInVertex, position), VK_FORMAT_R32G32B32_SFLOAT},
            GLVertexAttribute{1,offsetof(BuiltInVertex, normal), VK_FORMAT_R32G32B32_SFLOAT},
            GLVertexAttribute{2,offsetof(BuiltInVertex, tangent), VK_FORMAT_R32G32B32A32_SFLOAT},
            GLVertexAttribute{3,offsetof(BuiltInVertex, uv), VK_FORMAT_R32G32_SFLOAT},
            GLVertexAttribute{4,offsetof(BuiltInVertex, color), VK_FORMAT_R32G32B32A32_SFLOAT},
        }
    };
    const GLInputAssembly BuiltInGLInputAssembly = {
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        false
    };

    const GLMeshLayout BuiltInGLMeshLayout = {
        BuiltInGLVertexInput, BuiltInGLInputAssembly
    };

    class MeshBase
    {
    public:
        virtual ~MeshBase() = default;
        virtual void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const MeshBase* lastMesh) = 0;
        virtual const GLMeshLayout* GetGLMeshLayout() const = 0;
        virtual uint32_t GetIndexCount() const = 0;
    };

    class ShaderBase
    {
    public:
        virtual ~ShaderBase() = default;
        virtual void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const ShaderBase* lastShader) = 0;
        virtual const GLPipelineLayout& GetGLPipelineLayout() const = 0;
        virtual const std::vector<GLDescriptorBinding>& GetDescriptorBinding() const = 0;
        virtual const std::vector<VkPushConstantRange>& GetPushConstantBinding() const = 0;
    };

    class MaterialBase
    {
    public:
        virtual ~MaterialBase() = default;
        virtual void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const MaterialBase* lastMaterial) = 0;
        virtual const ShaderBase* GetShader() const = 0;
    };

    class RenderTextureBase
    {
    public:
        virtual ~RenderTextureBase() = default;
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual VkSampleCountFlagBits GetSampleCount() const = 0;
        virtual const VkImage& GetVKColorImage() const = 0;
        virtual const VkImage* GetVkDepthStencilImage() const = 0;
        virtual const VkImage* GetVkColorResolveImage() const = 0;
        virtual const GLImageView& GetGLColorImageView() const = 0;
        virtual const GLImageView* GetGLDepthStencilImageView() const = 0;
        virtual const GLImageView* GetGLColorResolveImageView() const = 0;
    };
}
