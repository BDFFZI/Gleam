#pragma once
#include <memory>

#include "LightGL/Runtime/Pipeline/GLMeshLayout.h"
#include "LightGL/Runtime/Resource/GLBuffer.h"
#include "LightMath/Runtime/Vector.hpp"

namespace Light
{
    class Mesh
    {
    public:
        virtual ~Mesh() = default;

        virtual const GLBuffer& GetGLVertexBuffer() const = 0;
        virtual const GLBuffer& GetGLIndexBuffer() const = 0;
        virtual size_t GetIndexCount() const = 0;

        virtual void UpdateGLBuffer() = 0;
    };

    template <class TVertex>
    class MeshTemplate : public Mesh
    {
    public:
        static const GLMeshLayout& GetMeshLayout();

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
}
