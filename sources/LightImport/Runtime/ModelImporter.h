#pragma once
#include <string>
#include <vector>

#include "LightMath/Runtime/Vector4.h"
#include "LightMath/Runtime/Color.h"

namespace LightRuntime
{
    struct RawMesh
    {
        std::vector<Vector3> positions;
        std::vector<Vector3> normals;
        std::vector<Vector4> tangents;
        std::vector<Vector2> uvs;
        std::vector<Color> colors;
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
