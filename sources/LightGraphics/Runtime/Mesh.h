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
        MeshT(const GLMeshLayout* glMeshLayout, const bool readwrite = false)
            : isDirty(true), glMeshLayout(glMeshLayout), readwrite(readwrite)
        {
        }

        std::vector<TVertex>& GetVertices()
        {
            return vertices;
        }
        std::vector<uint32_t>& GetIndices()
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
                if (readwrite == false)
                {
                    glVertexBuffer = std::make_unique<GLBuffer>(
                        sizeof(TVertex) * vertices.size(),
                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        vertices.data()
                    );
                    glIndexBuffer = std::make_unique<GLBuffer>(
                        sizeof(uint32_t) * indices.size(),
                        VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        indices.data()
                    );
                    vertices = {};
                    indices = {};
                }
                else
                {
                    size_t verticesSize = sizeof(TVertex) * vertices.size();
                    if (glVertexBuffer == nullptr || glVertexBuffer->size < verticesSize)
                        glVertexBuffer = std::make_unique<GLBuffer>(
                            verticesSize,
                            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                            vertices.data()
                        );
                    else
                    {
                        void* address = glVertexBuffer->MapMemory();
                        memcpy(address, vertices.data(), verticesSize);
                        glVertexBuffer->UnmapMemory();
                    }

                    size_t indicesSize = sizeof(uint32_t) * indices.size();
                    if (glIndexBuffer == nullptr || glIndexBuffer->size < indicesSize)
                        glIndexBuffer = std::make_unique<GLBuffer>(
                            indicesSize,
                            VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                            indices.data()
                        );
                    else
                    {
                        void* address = glIndexBuffer->MapMemory();
                        memcpy(address, indices.data(), indicesSize);
                        glIndexBuffer->UnmapMemory();
                    }
                }

                isDirty = false;
            }

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
        bool readwrite;
    };

    class Mesh : public MeshT<DefaultVertex>
    {
    public:
        static std::unique_ptr<Mesh> CreateFromRawMesh(const RawMesh& rawMesh);

        Mesh(const GLMeshLayout* glMeshLayout = &DefaultGLMeshLayout, const bool readwrite = false)
            : MeshT(glMeshLayout, readwrite)
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
