#include "Mesh.h"

namespace LightRuntime
{
    const GLMeshLayout& Mesh::GetMeshLayout()
    {
        return glMeshLayout;
    }
    const GLBuffer& Mesh::GetGLVertexBuffer() const
    {
        return *glVertexBuffer;
    }
    const GLBuffer& Mesh::GetGLIndexBuffer() const
    {
        return *glIndexBuffer;
    }
    int Mesh::GetIndexCount() const
    {
        return static_cast<int>(triangles.size());
    }

    int Mesh::GetVerticesCount() const
    {
        return static_cast<int>(vertices.size());
    }
    int Mesh::GetTrianglesCount() const
    {
        return static_cast<int>(triangles.size());
    }

#define GetProperty(functionName,propertyType,propertyName) \
    void Mesh::Get##functionName(std::vector<propertyType>& buffer) const \
    { \
        size_t size = vertices.size(); \
        buffer.resize(size); \
        for (size_t index = 0; index < size; index++) \
            buffer[index] = vertices[index].propertyName; \
    }

    GetProperty(Positions, float3, position)
    GetProperty(Normals, float3, normal)
    GetProperty(Tangents, float3, tangent)
    GetProperty(UVs, float2, uv)
    GetProperty(Colors, float4, color)
    void Mesh::GetVertices(std::vector<Vertex>& buffer) const
    {
        buffer = vertices;
    }
    void Mesh::GetTriangles(std::vector<uint32_t>& buffer) const
    {
        buffer = triangles;
    }

#define SetProperty(functionName,propertyType,propertyName) \
    void Mesh::Set##functionName(const std::vector<propertyType>& data) \
    { \
        const size_t size = data.size(); \
        if (vertices.size() != size) \
            vertices.resize(size); \
        for (size_t index = 0; index < size; index++) \
            vertices[index].propertyName = data[index]; \
    }

    SetProperty(Positions, float3, position)
    SetProperty(Normals, float3, normal)
    SetProperty(Tangents, float3, tangent)
    SetProperty(UVs, float2, uv)
    SetProperty(Colors, float4, color)
    void Mesh::SetVertices(const std::vector<Vertex>& data)
    {
        vertices = data;
    }
    void Mesh::SetTriangles(const std::vector<uint32_t>& data)
    {
        triangles = data;
    }

    void Mesh::UploadGL()
    {
        glVertexBuffer = std::make_unique<GLBuffer>(
            static_cast<int>(sizeof(Vertex) * vertices.size()),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            vertices.data()
        );
        glIndexBuffer = std::make_unique<GLBuffer>(
            static_cast<int>(sizeof(uint32_t) * triangles.size()),
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            triangles.data()
        );
    }
}
