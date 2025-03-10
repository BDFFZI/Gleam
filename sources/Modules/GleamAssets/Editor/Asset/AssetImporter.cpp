#include "AssetImporter.h"

namespace Gleam
{
    void AssetImporter::AddCustomImporter(const std::string_view extension, const Type& type)
    {
        customImporters.emplace(std::string(extension), &type);
    }
    bool AssetImporter::HasImporter(const std::filesystem::path& assetPath)
    {
        std::string extension = assetPath.extension().string();
        return customImporters.contains(extension);
    }
    // AssetImporter& AssetImporter::GetImporter(const std::string& assetPath)
    // {
    //     std::filesystem::path importerPath = assetPath + ".meta";
    //     if (exists(importerPath))
    //     {
    //         //加载已有导入器
    //     }
    //     else
    //     {
    //         //创建导入器
    //         std::string extension = std::filesystem::path(assetPath).extension().string();
    //         const Type& importerType = *customImporters.at(extension);
    //     }
    // }
    // AssetBundle& AssetImporter::ImportAsset()
    // {
    //     
    // }
    // void AssetImporter::SaveAndReimport()
    // {
    // }
}