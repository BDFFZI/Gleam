#pragma once
#include <memory>

#include "GraphicsConfig.hpp"
#include "LightGL/Runtime/Resource/GLBuffer.h"
#include "LightImport/Runtime/ModelImporter.h"
#include "LightMath/Runtime/Vector.hpp"
#include <cassert>

namespace Light
{
    template <class TVertex>
    class MeshT : public MeshBase
    {
    public:
        /**
         * 
         * @param glMeshLayout 
         * @param readwrite 网格是否可读写。若不可读写，资源上传到GPU后，CPU内存将被清除，相关读写功能也不可用。
         */
        MeshT(const GLMeshLayout* glMeshLayout, const bool readwrite = false)
            : isDirty(true), glMeshLayout(glMeshLayout), glIndexCount(0), readwrite(readwrite)
        {
        }

        /**
         * 返回Mesh内部使用的CPU顶点缓冲区内存。
         * 
         * 如果你直接修改了其内容，必须显式调用@c SetDirty 以重新上传到GPU。
         * @return 
         */
        std::vector<TVertex>& GetVertices()
        {
            return vertices;
        }
        /**
         * 返回Mesh内部使用的CPU索引缓冲区内存。
         * 
         * 如果你直接修改了其内容，必须显式调用@c SetDirty 以重新上传到GPU。
         * @return 
         */
        std::vector<uint32_t>& GetIndices()
        {
            return indices;
        }
        void SetVertices(const std::vector<TVertex>& data)
        {
            vertices.resize(data.size());
            std::ranges::copy(data, vertices.data());
            isDirty = true;
        }
        void SetIndices(const std::vector<uint32_t>& data)
        {
            indices.resize(data.size());
            std::ranges::copy(data, indices.data());
            isDirty = true;
        }
        void SetDirty() { isDirty = true; }

        const GLMeshLayout* GetGLMeshLayout() const override { return glMeshLayout; }
        uint32_t GetGLIndexCount() const override { return glIndexCount; }

        void BindToPipeline(const GLCommandBuffer& commandBuffer, const MeshBase* lastMesh) override
        {
            if (isDirty)
            {
                assert(!vertices.empty() && "未设置任何顶点，网格数据无效");
                assert(!indices.empty() && "未设置任何索引，网格数据无效");

                glIndexCount = static_cast<uint32_t>(indices.size());

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
        uint32_t glIndexCount;
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
