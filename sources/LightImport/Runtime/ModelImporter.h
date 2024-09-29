#pragma once
#include <string>
#include <vector>

#include "LightMath/Runtime/Graphics.h"
#include "LightMath/Runtime/Vector.h"

namespace LightRuntime
{
    struct RawMesh
    {
        std::vector<float3> positions;
        std::vector<float3> normals;
        std::vector<float4> tangents;
        std::vector<float2> uvs;
        std::vector<color> colors;
        std::vector<uint32_t> triangles;
    };

    class ModelImporter
    {
    public:
        static RawMesh ImportObj(const std::string& filePath);
    };
}
