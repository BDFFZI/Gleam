#pragma once
#include "CommandBuffer.h"
#include "LightGL/Runtime/Pipeline/GLMeshLayout.h"
#include "LightGL/Runtime/Pipeline/GLPipeline.h"
#include "LightGL/Runtime/Resource/GLBuffer.h"
#include "LightMath/Runtime/Matrix.hpp"
#include "LightMath/Runtime/Vector.hpp"


namespace Light
{
    struct PushConstant
    {
        alignas(16) float4x4 objectToWorld;
        alignas(16) float4x4 worldToView;
        alignas(16) float4x4 viewToClip;
    };

    const StateLayout DefaultStateLayout = []
    {
        StateLayout stateLayout = {};
        //解耦着色器与抗锯齿的关系，根据渲染目标属性实时调整
        stateLayout.dynamicStates.push_back(VK_DYNAMIC_STATE_RASTERIZATION_SAMPLES_EXT);
        //解耦着色器与网格基元的关系，实现不同基元网格共用着色器
        stateLayout.dynamicStates.push_back(VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY);

        return stateLayout;
    }();

    struct Vertex_Base
    {
        float3 position;
        float4 color;
    };

    const GLMeshVertexLayout DefaultVertexLayout_Base = {
        sizeof(Vertex_Base), {
            GLVertexAttribute{0,offsetof(Vertex_Base, position), VK_FORMAT_R32G32B32_SFLOAT},
            GLVertexAttribute{4,offsetof(Vertex_Base, color), VK_FORMAT_R32G32B32A32_SFLOAT},
        }
    };

    struct Vertex
    {
        float3 position;
        float3 normal;
        float4 tangent;
        float2 uv;
        float4 color;
    };

    const GLMeshVertexLayout DefaultVertexLayout = {
        sizeof(Vertex), {
            GLVertexAttribute{0,offsetof(Vertex, position), VK_FORMAT_R32G32B32_SFLOAT},
            GLVertexAttribute{1,offsetof(Vertex, normal), VK_FORMAT_R32G32B32_SFLOAT},
            GLVertexAttribute{2,offsetof(Vertex, tangent), VK_FORMAT_R32G32B32A32_SFLOAT},
            GLVertexAttribute{3,offsetof(Vertex, uv), VK_FORMAT_R32G32_SFLOAT},
            GLVertexAttribute{4,offsetof(Vertex, color), VK_FORMAT_R32G32B32A32_SFLOAT},
        }
    };

    const GLMeshLayout DefaultMeshLayout = {
        DefaultVertexLayout, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
    };

    class MeshBase
    {
    public:
        virtual ~MeshBase() = default;

        virtual const GLBuffer& GetGLVertexBuffer() const = 0;
        virtual const GLBuffer& GetGLIndexBuffer() const = 0;
        virtual size_t GetIndexCount() const = 0;
        virtual VkPrimitiveTopology GetPrimitiveTopology() const = 0;

        virtual void 
        virtual void BindToPipeline(const CommandBuffer&) = 0;
    };
}
