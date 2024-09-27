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

    RawImage ImportPng(const std::string& filePath);
}
