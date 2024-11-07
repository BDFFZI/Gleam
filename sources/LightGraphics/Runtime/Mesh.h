#pragma once
#include <memory>

#include "Preset.hpp"
#include "LightGL/Runtime/Resource/GLBuffer.h"
#include "LightImport/Runtime/ModelImporter.h"
#include "LightMath/Runtime/Vector.hpp"

namespace Light
{
    template <class TVertex>
    class MeshTemplate : public MeshBase
    {
    public:
        MeshTemplate(const VkPrimitiveTopology primitiveTopology): primitiveTopology(primitiveTopology)
        {
        }

        const std::vector<TVertex>& GetVertices() const
        {
            return vertices;
        }
        const std::vector<uint32_t>& GetIndices() const
        {
            return indices;
        }
        void SetVertices(std::vector<TVertex> data)
        {
            vertices = std::move(data);
        }
        void SetIndices(std::vector<uint32_t> data)
        {
            indices = std::move(data);
        }

        const GLBuffer& GetGLVertexBuffer() const override { return *glVertexBuffer; }
        const GLBuffer& GetGLIndexBuffer() const override { return *glIndexBuffer; }
        size_t GetVertexCount() const { return vertices.size(); }
        size_t GetIndexCount() const override { return indices.size(); }
        VkPrimitiveTopology GetPrimitiveTopology() const override { return primitiveTopology; }

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

    protected:
        std::vector<TVertex> vertices;
        std::vector<uint32_t> indices;

    private:
        std::unique_ptr<GLBuffer> glVertexBuffer;
        std::unique_ptr<GLBuffer> glIndexBuffer;
        VkPrimitiveTopology primitiveTopology;
    };

    class Mesh : public MeshTemplate<Vertex>
    {
    public:
        static std::unique_ptr<Mesh> CreateFromRawMesh(const RawMesh& rawMesh);

        Mesh(const VkPrimitiveTopology primitiveTopology = DefaultMeshLayout.inputAssembly.topology): MeshTemplate(primitiveTopology)
        {
        }
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
