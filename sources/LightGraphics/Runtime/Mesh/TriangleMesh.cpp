#include "TriangleMesh.h"

#include "LightImport/Runtime/ModelImporter.h"

namespace Light
{
    std::unique_ptr<TriangleMesh> TriangleMesh::CreateFromRawMesh(const RawMesh& rawMesh)
    {
        TriangleMesh* mesh = new TriangleMesh();
        mesh->SetPositions(rawMesh.positions);
        mesh->SetNormals(rawMesh.normals);
        mesh->SetTangents(rawMesh.tangents);
        mesh->SetUVs(rawMesh.uvs);
        mesh->SetColors(rawMesh.colors);
        mesh->SetIndices(rawMesh.triangles);
        mesh->UpdateGLBuffer();
        return std::unique_ptr<TriangleMesh>(mesh);
    }

#define GetProperty(functionName,propertyType,propertyName) \
    void TriangleMesh::Get##functionName(std::vector<propertyType>& buffer) const \
    { \
        size_t size = vertices.size(); \
        buffer.resize(size); \
        for (size_t index = 0; index < size; index++) \
            buffer[index] = vertices[index].propertyName; \
    }

    GetProperty(Positions, float3, position)
    GetProperty(Normals, float3, normal)
    GetProperty(Tangents, float4, tangent)
    GetProperty(UVs, float2, uv)
    GetProperty(Colors, float4, color)

#define SetProperty(functionName,propertyType,propertyName) \
    void TriangleMesh::Set##functionName(const std::vector<propertyType>& data) \
    { \
        const size_t size = data.size(); \
        if (vertices.size() != size) \
            vertices.resize(size); \
        for (size_t index = 0; index < size; index++) \
            vertices[index].propertyName = data[index]; \
    }

    SetProperty(Positions, float3, position)
    SetProperty(Normals, float3, normal)
    SetProperty(Tangents, float4, tangent)
    SetProperty(UVs, float2, uv)
    SetProperty(Colors, float4, color)
}
