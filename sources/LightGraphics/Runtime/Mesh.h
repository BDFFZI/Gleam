#pragma once
#include <memory>

#include "LightGL/Runtime/Pipeline/GLMeshLayout.h"
#include "LightGL/Runtime/Resource/GLBuffer.h"
#include "LightImport/Runtime/ModelImporter.h"
#include "LightMath/Runtime/Vector.hpp"

namespace Light
{
    class MeshBase
    {
    public:
        virtual ~MeshBase() = default;

        virtual const GLBuffer& GetGLVertexBuffer() const = 0;
        virtual const GLBuffer& GetGLIndexBuffer() const = 0;
        virtual size_t GetIndexCount() const = 0;

        virtual void UpdateGLBuffer() = 0;
    };

    template <class TVertex>
    class MeshTemplate : public MeshBase
    {
    public:
        void GetVertices(std::vector<TVertex>& buffer) const
        {
            buffer = vertices;
        }
        void GetIndices(std::vector<uint32_t>& buffer) const
        {
            buffer = indices;
        }
        void SetVertices(const std::vector<TVertex>& data)
        {
            vertices = data;
        }
        void SetIndices(const std::vector<uint32_t>& data)
        {
            indices = data;
        }

        void UpdateGLBuffer() override
        {
            glVertexBuffer = std::make_unique<GLBuffer>(
                static_cast<int>(sizeof(TVertex) * vertices.size()),
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                vertices.data()
            );
            glIndexBuffer = std::make_unique<GLBuffer>(
                static_cast<int>(sizeof(uint32_t) * indices.size()),
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                indices.data()
            );
        }
        const GLBuffer& GetGLVertexBuffer() const override
        {
            return *glVertexBuffer;
        }
        const GLBuffer& GetGLIndexBuffer() const override
        {
            return *glIndexBuffer;
        }
        size_t GetVertexCount() const
        {
            return vertices.size();
        }
        size_t GetIndexCount() const override
        {
            return indices.size();
        }

    protected:
        std::vector<TVertex> vertices;
        std::vector<uint32_t> indices;

    private:
        std::unique_ptr<GLBuffer> glVertexBuffer;
        std::unique_ptr<GLBuffer> glIndexBuffer;
    };

    struct Vertex
    {
        float3 position;
        float3 normal;
        float4 tangent;
        float2 uv;
        float4 color;
    };

    class Mesh : public MeshTemplate<Vertex>
    {
    public:
        static std::unique_ptr<Mesh> CreateFromRawMesh(const RawMesh& rawMesh);
        static const GLMeshLayout& GetMeshLayout();

        Mesh() = default;
        Mesh(const Mesh&) = delete;

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
