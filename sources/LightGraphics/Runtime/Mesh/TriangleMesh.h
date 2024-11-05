#pragma once
#include "Mesh.h"
#include "LightMath/Runtime/Vector.hpp"

namespace Light
{
    struct TriangleVertex
    {
        float3 position;
        float3 normal;
        float4 tangent;
        float2 uv;
        float4 color;
    };

    inline GLMeshLayout triangleMeshLayout = {
        sizeof(TriangleVertex), {
            GLVertexAttribute{offsetof(TriangleVertex, position), VK_FORMAT_R32G32B32_SFLOAT},
            GLVertexAttribute{offsetof(TriangleVertex, normal), VK_FORMAT_R32G32B32_SFLOAT},
            GLVertexAttribute{offsetof(TriangleVertex, tangent), VK_FORMAT_R32G32B32_SFLOAT},
            GLVertexAttribute{offsetof(TriangleVertex, uv), VK_FORMAT_R32G32_SFLOAT},
            GLVertexAttribute{offsetof(TriangleVertex, color), VK_FORMAT_R32G32B32A32_SFLOAT},
        },
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
    };
    template <>
    inline const GLMeshLayout& MeshTemplate<TriangleVertex>::GetMeshLayout()
    {
        return triangleMeshLayout;
    }

    class TriangleMesh : public MeshTemplate<TriangleVertex>
    {
    public:
        TriangleMesh() = default;
        TriangleMesh(const TriangleMesh&) = delete;

        void GetPositions(std::vector<float3>& buffer) const;
        void GetNormals(std::vector<float3>& buffer) const;
        void GetTangents(std::vector<float4>& buffer) const;
        void GetUVs(std::vector<float2>& buffer) const;
        void GetColors(std::vector<float4>& buffer) const;

        void SetPositions(const std::vector<float3>& data);
        void SetNormals(const std::vector<float3>& data);
        void SetTangents(const std::vector<float4>& data);
        void SetUVs(const std::vector<float2>& data);
        void SetColors(const std::vector<float4>& data);
    };
}
