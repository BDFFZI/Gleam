#pragma once
#include <memory>

#include "LightGL/Runtime/Resource/GLBuffer.h"
#include "LightMath/Runtime/Vector.h"

namespace LightRuntime
{
    class MeshBase
    {
    public:
        virtual ~MeshBase() = default;

        virtual const GLBuffer& GetGLVertexBuffer() = 0;
        virtual const GLBuffer& GetGLIndexBuffer() = 0;
    };

    struct Vertex
    {
        float3 position;
        float3 normal;
        float3 tangent;
        float2 uv;
        float4 color;
    };

    class Mesh : public MeshBase
    {
    public:
        const GLBuffer& GetGLVertexBuffer() override;
        const GLBuffer& GetGLIndexBuffer() override;

        int GetVerticesCount() const;
        int GetTrianglesCount() const;

        void GetPositions(std::vector<float3>& buffer) const;
        void GetNormals(std::vector<float3>& buffer) const;
        void GetTangents(std::vector<float3>& buffer) const;
        void GetUVs(std::vector<float2>& buffer) const;
        void GetColors(std::vector<float4>& buffer) const;
        void GetVertices(std::vector<Vertex>& buffer) const;
        void GetTriangles(std::vector<uint32_t>& buffer) const;

        void SetPositions(const std::vector<float3>& data);
        void SetNormals(const std::vector<float3>& data);
        void SetTangents(const std::vector<float3>& data);
        void SetUVs(const std::vector<float2>& data);
        void SetColors(const std::vector<float4>& data);
        void SetVertices(const std::vector<Vertex>& data);
        void SetTriangles(const std::vector<uint32_t>& data);

        void UploadGL();

    private:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> triangles;
        std::unique_ptr<GLBuffer> glVertexBuffer;
        std::unique_ptr<GLBuffer> glIndexBuffer;
    };
}
