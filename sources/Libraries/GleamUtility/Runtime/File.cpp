#include "File.h"

#include <filesystem>
#include <stdexcept>
#include <fstream>

namespace Gleam
{
    void File::WriteAllText(const std::filesystem::path& filePath, const std::string_view content)
    {
        auto rootDirectory = std::filesystem::path(filePath).parent_path();
        if (!rootDirectory.empty() && !exists(rootDirectory))
            create_directories(rootDirectory);

        std::ofstream file(filePath, std::ios::binary);
        file.write(content.data(), static_cast<std::streamsize>(content.size()));
        file.close();
    }
    std::string File::ReadAllText(const std::filesystem::path& filePath)
    {
        //通过ate标志初始就将读取位置设在流末尾
        std::ifstream file(filePath, std::ios::ate | std::ios::binary);
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
    void File::OpenInExplorer(const std::filesystem::path& filePath)
    {
        std::filesystem::path platformPath(filePath);
        platformPath.make_preferred();
#ifdef GleamPlatformWindows
        std::system(std::format("explorer.exe /select,{}", platformPath.string()).c_str());
#endif
    }
    void File::OpenInAssociatedApp(const std::filesystem::path& filePath)
    {
        std::filesystem::path platformPath(filePath);
        platformPath.make_preferred();
#ifdef GleamPlatformWindows
        if (is_directory(filePath))
            std::system(std::format("explorer.exe {}", platformPath.string()).c_str());
        else
            std::system(std::format("start {}", platformPath.string()).c_str());
#endif
    }
}