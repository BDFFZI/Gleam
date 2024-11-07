#pragma once
#include <memory>

#include "GraphicsConfig.hpp"
#include "LightGL/Runtime/Resource/GLBuffer.h"
#include "LightImport/Runtime/ModelImporter.h"
#include "LightMath/Runtime/Vector.hpp"

namespace Light
{
    template <class TVertex>
    class MeshT : public MeshBase
    {
    public:
        MeshT(const GLMeshLayout* glMeshLayout)
            : isDirty(true), glMeshLayout(glMeshLayout)
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
            isDirty = true;
        }
        void SetIndices(std::vector<uint32_t> data)
        {
            indices = std::move(data);
            isDirty = true;
        }

        const GLMeshLayout* GetGLMeshLayout() const override { return glMeshLayout; }
        uint32_t GetIndexCount() const override { return static_cast<uint32_t>(indices.size()); }

        void BindToPipeline(const GLCommandBuffer& commandBuffer, const MeshBase* lastMesh) override
        {
            if (isDirty)
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

            // if (lastMesh == nullptr || lastMesh->GetGLMeshLayout() != glMeshLayout)
            // {
            //     commandBuffer.SetVertexInput(glMeshLayout->vertexInput);
            //     commandBuffer.SetInputAssembly(glMeshLayout->inputAssembly);
            // }
            commandBuffer.BindVertexBuffers(*glVertexBuffer);
            commandBuffer.BindIndexBuffer(*glIndexBuffer);
        }

    protected:
        std::vector<TVertex> vertices;
        std::vector<uint32_t> indices;
        bool isDirty;

    private:
        std::unique_ptr<GLBuffer> glVertexBuffer;
        std::unique_ptr<GLBuffer> glIndexBuffer;
        const GLMeshLayout* glMeshLayout;
    };

    class Mesh : public MeshT<BuiltInVertex>
    {
    public:
        static std::unique_ptr<Mesh> CreateFromRawMesh(const RawMesh& rawMesh);

        Mesh(const GLMeshLayout* glMeshLayout = &BuiltInGLMeshLayout)
            : MeshT(glMeshLayout)
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
