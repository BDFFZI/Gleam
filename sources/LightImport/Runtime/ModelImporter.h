#pragma once
#include <string>
#include <vector>

#include "LightMath/Runtime/Vector.hpp"

namespace LightRuntime
{
    struct RawMesh
    {
        std::vector<float3> positions;
        std::vector<float3> normals;
        std::vector<float4> tangents;
        std::vector<float2> uvs;
        std::vector<float4> colors;
        std::vector<uint32_t> triangles;
    };

    class ModelImporter
    {
    public:
        /**
         * 采用tiny_obj_loader加载obj文件。
         * @param filePath
         * @return 
         */
        static RawMesh ImportObj(const std::string& filePath);
    };
}
