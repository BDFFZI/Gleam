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
        /**
         * 采用tiny_obj_loader加载obj文件。
         * 注意：obj文件的三角形索引顺序是逆时针，这与常见图形渲染的要求并不一致。
         * @param filePath 
         * @return 
         */
        static RawMesh ImportObj(const std::string& filePath);
    };
}
