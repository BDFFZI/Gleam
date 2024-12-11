#pragma once
#include <memory>

#include "LightGL/Runtime/Resource/GLBuffer.h"
#include "LightImport/Runtime/ModelImporter.h"
#include <cassert>

#include "LightGL/Runtime/Pipeline/GLCommandBuffer.h"

namespace Light
{
    class MeshBase
    {
    public:
        virtual ~MeshBase() = default;
        virtual void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const MeshBase* lastMesh) = 0;
        virtual const GLMeshLayout* GetGLMeshLayout() const = 0;
        /**
         * GLIndexCount等于索引数量，但并非总是有效。因为它是专供图形接口使用的索引数量，因此只有在绑定资源到管线时才被计算。
         * @return 
         */
        virtual uint32_t GetGLIndexCount() const = 0;
    };

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
}
