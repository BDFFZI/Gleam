#pragma once
#include <string>
#include <vector>

namespace LightRuntime
{
    struct RawImage
    {
        int width;
        int height;
        std::vector<std::byte> pixels;
    };

    class ImageImporter
    {
    public:
        static RawImage Import(const std::string& filePath);
    };
}
