#include "File.h"

#include <filesystem>
#include <stdexcept>
#include <fstream>

namespace Gleam
{
    void File::WriteAllText(const std::string_view filename, const std::string_view content)
    {
        auto rootDirectory = std::filesystem::path(filename).parent_path();
        if (!exists(rootDirectory))create_directory(rootDirectory);

        std::ofstream file(filename.data(), std::ios::binary);
        file.write(content.data(), static_cast<std::streamsize>(content.size()));
        file.close();
    }
    std::string File::ReadAllText(const std::string_view filename)
    {
        //通过ate标志初始就将读取位置设在流末尾
        std::ifstream file(filename.data(), std::ios::ate | std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error("文件打开失败！");

        //由于读取位置在流末尾，故其位置即文件长度
        const std::streamsize fileSize = file.tellg();
        std::string content(fileSize, '0');

        //读取内容
        file.seekg(0);
        file.read(content.data(), fileSize);

        file.close();

        return content;
    }
}