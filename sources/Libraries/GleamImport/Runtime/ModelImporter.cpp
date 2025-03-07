﻿#include "ModelImporter.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <unordered_map>

#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"

using namespace Gleam;

struct ObjVertex
{
    float3 pos;
    float2 texCoord;
    float4 color;

    bool operator==(const ObjVertex& other) const
    {
        return all(pos == other.pos) && all(texCoord == other.texCoord);
    }
};

template <>
struct std::hash<ObjVertex>
{
    size_t operator()(ObjVertex const& vertex) const noexcept
    {
        return hash<float3>()(vertex.pos) >> 1 ^ hash<float2>()(vertex.texCoord) << 1;
    }
};

RawMesh ModelImporter::ImportObj(const std::string& filePath)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    if (!LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str()))
        throw std::runtime_error(warn + err);

    RawMesh mesh = {};
    std::unordered_map<ObjVertex, uint32_t> uniqueVertices{};
    
    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            ObjVertex vertex{};
            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = {
                attrib.colors[3 * index.vertex_index + 0],
                attrib.colors[3 * index.vertex_index + 1],
                attrib.colors[3 * index.vertex_index + 2],
                1,
            };

            if (!uniqueVertices.contains(vertex))
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(mesh.positions.size());
                mesh.positions.push_back(vertex.pos);
                mesh.uvs.push_back(vertex.texCoord);
                mesh.colors.push_back(vertex.color);
            }

            mesh.triangles.push_back(uniqueVertices[vertex]);
        }
    }

    return mesh;
}
