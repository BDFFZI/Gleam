#pragma once
#include <memory>

#include "LightGL/Runtime/Pipeline/GLMeshLayout.h"
#include "LightGL/Runtime/Resource/GLBuffer.h"
#include "LightMath/Runtime/Color.h"
#include "LightMath/Runtime/Vector4.h"

namespace LightRuntime
{
    class MeshBase
    {
    public:
        virtual ~MeshBase() = default;

        virtual const GLBuffer& GetGLVertexBuffer() const = 0;
        virtual const GLBuffer& GetGLIndexBuffer() const = 0;
        virtual int GetIndexCount() const = 0;

        virtual void UpdateGLBuffer() = 0;
    };

    struct Vertex
    {
        Vector3 position;
        Vector3 normal;
        Vector4 tangent;
        Vector2 uv;
        Color color;
    };

    class Mesh : public MeshBase
    {
    public:
        Mesh() = default;
        Mesh(const Mesh&) = delete;

        static const GLMeshLayout& GetMeshLayout();

        const GLBuffer& GetGLVertexBuffer() const override;
        const GLBuffer& GetGLIndexBuffer() const override;
        int GetIndexCount() const override;

        int GetVerticesCount() const;
        int GetTrianglesCount() const;

        void GetPositions(std::vector<Vector3>& buffer) const;
        void GetNormals(std::vector<Vector3>& buffer) const;
        void GetTangents(std::vector<Vector4>& buffer) const;
        void GetUVs(std::vector<Vector2>& buffer) const;
        void GetColors(std::vector<Color>& buffer) const;
        void GetVertices(std::vector<Vertex>& buffer) const;
        void GetTriangles(std::vector<uint32_t>& buffer) const;

        void SetPositions(const std::vector<Vector3>& data);
        void SetNormals(const std::vector<Vector3>& data);
        void SetTangents(const std::vector<Vector4>& data);
        void SetUVs(const std::vector<Vector2>& data);
        void SetColors(const std::vector<Color>& data);
        void SetVertices(const std::vector<Vertex>& data);
        void SetTriangles(const std::vector<uint32_t>& data);

        void UpdateGLBuffer() override;

    private:
        static inline GLMeshLayout glMeshLayout = {
            sizeof(Vertex), {
                GLVertexAttribute{offsetof(Vertex, position), VK_FORMAT_R32G32B32_SFLOAT},
                GLVertexAttribute{offsetof(Vertex, normal), VK_FORMAT_R32G32B32_SFLOAT},
                GLVertexAttribute{offsetof(Vertex, tangent), VK_FORMAT_R32G32B32_SFLOAT},
                GLVertexAttribute{offsetof(Vertex, uv), VK_FORMAT_R32G32_SFLOAT},
                GLVertexAttribute{offsetof(Vertex, color), VK_FORMAT_R32G32B32A32_SFLOAT},
            },
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
        };

        std::vector<Vertex> vertices;
        std::vector<uint32_t> triangles;
        std::unique_ptr<GLBuffer> glVertexBuffer;
        std::unique_ptr<GLBuffer> glIndexBuffer;
    };
}
