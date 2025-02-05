#include "Mesh.h"

namespace Light
{
    Mesh::Mesh(const bool readwrite): GMeshT(readwrite)
    {
    }
    Mesh::Mesh(const RawMesh& rawMesh, const bool readwrite): Mesh(readwrite)
    {
        SetPositions(rawMesh.positions);
        SetIndices(rawMesh.triangles);

        if (!rawMesh.normals.empty())SetNormals(rawMesh.normals);
        if (!rawMesh.tangents.empty())SetTangents(rawMesh.tangents);
        if (!rawMesh.uvs.empty()) SetUVs(rawMesh.uvs);
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
    GetProperty(Colors, float4, color)
    GetProperty(Normals, float3, normal)
    GetProperty(Tangents, float3, tangent)
    GetProperty(UVs, float2, uv)

#define SetProperty(functionName,propertyType,propertyName) \
void Mesh::Set##functionName(const std::vector<propertyType>& data) \
{ \
const size_t size = data.size(); \
if (vertices.size() != size) \
vertices.resize(size); \
for (size_t index = 0; index < size; index++) \
vertices[index].propertyName = data[index]; \
SetDirty();\
}

    SetProperty(Positions, float3, position)
    SetProperty(Colors, float4, color)
    SetProperty(Normals, float3, normal)
    SetProperty(Tangents, float3, tangent)
    SetProperty(UVs, float2, uv)
}
