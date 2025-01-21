#pragma once
#include "LightGL/Runtime/Pipeline/GLMeshLayout.h"
#include "LightMath/Runtime/Vector.hpp"

namespace Light
{
    //网格布局
    struct Vertex
    {
        float3 position;
        //构建法线空间
        float3 normal;
        float3 tangent;
        //纹理采样
        float2 uv;
    };

    inline static const GLVertexInput VertexInput = {
        sizeof(Vertex), {
            GLVertexAttribute{0,offsetof(Vertex, position), VK_FORMAT_R32G32B32_SFLOAT},
            GLVertexAttribute{1,offsetof(Vertex, normal), VK_FORMAT_R32G32B32_SFLOAT},
            GLVertexAttribute{2,offsetof(Vertex, tangent), VK_FORMAT_R32G32B32_SFLOAT},
            GLVertexAttribute{3,offsetof(Vertex, uv), VK_FORMAT_R32G32_SFLOAT},
        }
    };
    inline static const GLInputAssembly InputAssembly = {
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        false
    };
    inline static const GLMeshLayout MeshLayout = {
        VertexInput, InputAssembly
    };
    
    class Mesh : public MeshT<GraphicsPreset::Vertex>
    {
    public:
        Mesh(const bool readwrite = false): MeshT(readwrite)
        {
        }
        Mesh(const RawMesh& rawMesh, bool readwrite = false);
        Mesh(const Mesh&) = delete;

        void GetPositions(std::vector<float3>& buffer) const;
        void GetNormals(std::vector<float3>& buffer) const;
        void GetTangents(std::vector<float3>& buffer) const;
        void GetUVs(std::vector<float2>& buffer) const;

        void SetPositions(const std::vector<float3>& data);
        void SetNormals(const std::vector<float3>& data);
        void SetTangents(const std::vector<float3>& data);
        void SetUVs(const std::vector<float2>& data);
    };
}
