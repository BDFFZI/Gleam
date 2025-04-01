#pragma once
#include <filesystem>
#include <string>

namespace Gleam
{
    class File
    {
    public:
        static void WriteAllText(const std::filesystem::path& filePath, std::string_view content);
        static std::string ReadAllText(const std::filesystem::path& filePath);
        static void OpenInExplorer(const std::filesystem::path& filePath);
        static void OpenInAssociatedApp(const std::filesystem::path& filePath);
    };
}