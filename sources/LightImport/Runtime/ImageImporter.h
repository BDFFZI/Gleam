#pragma once
#include <stb_image.h>
#include <string>
#include <vector>

namespace Light
{
    struct RawImage
    {
        int width;
        int height;
        int channel;
        std::vector<std::byte> pixels;
    };

    class ImageImporter
    {
    public:
        /**
         * 利用stb_image加载多种格式图片。
         * 注意：像素加载顺序从左上角开始逐行扫描，因此第一个像素的uv是(0,1)，而不是(0,0)，这对一些图形API的uv识别会产生影响。
         * @param filePath
         * @param desiredChannel 
         * @return 
         */
        static RawImage Import(const std::string& filePath, int desiredChannel = STBI_default);
    };
}
