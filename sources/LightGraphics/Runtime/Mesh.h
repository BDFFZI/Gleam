#pragma once
#include <memory>

#include "LightGL/Runtime/Resource/GLBuffer.h"
#include "LightImport/Runtime/ModelImporter.h"
#include <cassert>

#include "GraphicsAssets.h"
#include "GraphicsPreset.h"
#include "LightGL/Runtime/Pipeline/GLCommandBuffer.h"

namespace Light
{
    template <class TVertex>
    class MeshT : public MeshAsset
    {
    public:
        /**
         * 
         * @param readwrite 网格是否可读写。若不可读写，资源上传到GPU后，CPU内存将被清除，相关读写功能也不可用。
         */
        MeshT(const bool readwrite = false)
            : isDirty(true), readwrite(readwrite)
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

        void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const MeshAsset* lastMesh) override
        {
            if (isDirty)
            {
                UploadMeshData();
                isDirty = false;
                MeshAsset::BindToPipeline(glCommandBuffer, lastMesh);
            }
            else if (this != lastMesh)
            {
                MeshAsset::BindToPipeline(glCommandBuffer, lastMesh);
            }
        }

    protected:
        std::vector<TVertex> vertices;
        std::vector<uint32_t> indices;

    private:
        bool isDirty;
        bool readwrite;
        std::unique_ptr<GLBuffer> vertexBuffer;
        std::unique_ptr<GLBuffer> indexBuffer;

        /**
         * 将对Mesh的CPU端数据的所有改动上传到GPU端
         */
        void UploadMeshData()
        {
            assert(!vertices.empty() && "未设置任何顶点，网格数据无效");
            assert(!indices.empty() && "未设置任何索引，网格数据无效");

            glIndexCount = static_cast<uint32_t>(indices.size());//indices有可能被清空，需提前设置
            
            if (readwrite == false)
            {
                vertexBuffer = std::make_unique<GLBuffer>(
                    sizeof(TVertex) * vertices.size(),
                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    vertices.data()
                );
                indexBuffer = std::make_unique<GLBuffer>(
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
                if (vertexBuffer == nullptr || vertexBuffer->size < verticesSize)
                    vertexBuffer = std::make_unique<GLBuffer>(
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
                if (indexBuffer == nullptr || indexBuffer->size < indicesSize)
                    indexBuffer = std::make_unique<GLBuffer>(
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

            glVertexBuffer = vertexBuffer.get();
            glIndexBuffer = indexBuffer.get();
        }
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
