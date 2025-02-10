#pragma once
#include <memory>

#include "GleamGL/Runtime/Resource/GLBuffer.h"
#include <cassert>

#include "GleamGL/Runtime/GLCommandBuffer.h"
#include "GleamImport/Runtime/ModelImporter.h"

namespace Gleam
{
    class GMesh
    {
    public:
        virtual ~GMesh() = default;
        /**
         * 注意！只有在调用BindToPipeline后该函数才有效
         * @return 
         */
        virtual uint32_t GetGLIndexCount() = 0;
        virtual void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const GMesh* currentMesh) = 0;
    };

    template <class TVertex>
    class GMeshT : public GMesh
    {
    public:
        /**
         * 
         * @param readwrite 网格是否可读写。若不可读写，资源上传到GPU后，CPU内存将被清除，相关读写功能也不可用。
         */
        GMeshT(const bool readwrite = false)
            : readwrite(readwrite)
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

        uint32_t GetGLIndexCount() override { return indexCount; }
        void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const GMesh* lastMesh) override
        {
            if (isDirty)
            {
                UploadMeshData();
                isDirty = false;

                glCommandBuffer.BindVertexBuffers(*vertexBuffer);
                glCommandBuffer.BindIndexBuffer(*indexBuffer);
            }
            else if (this != lastMesh)
            {
                glCommandBuffer.BindVertexBuffers(*vertexBuffer);
                glCommandBuffer.BindIndexBuffer(*indexBuffer);
            }
        }

    protected:
        std::vector<TVertex> vertices;
        std::vector<uint32_t> indices;

    private:
        bool readwrite;
        bool isDirty = true;
        std::unique_ptr<GLBuffer> vertexBuffer = nullptr;
        std::unique_ptr<GLBuffer> indexBuffer = nullptr;
        uint32_t indexCount = 0;

        /**
         * 将对Mesh的CPU端数据的所有改动上传到GPU端
         */
        void UploadMeshData()
        {
            assert(!vertices.empty() && "未设置任何顶点，网格数据无效");
            assert(!indices.empty() && "未设置任何索引，网格数据无效");

            indexCount = static_cast<uint32_t>(indices.size()); //indices有可能被清空，需提前设置

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
                    void* address = vertexBuffer->MapMemory();
                    memcpy(address, vertices.data(), verticesSize);
                    vertexBuffer->UnmapMemory();
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
                    void* address = indexBuffer->MapMemory();
                    memcpy(address, indices.data(), indicesSize);
                    indexBuffer->UnmapMemory();
                }
            }
        }
    };
}