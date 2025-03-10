#include "AssetImporter.h"

namespace Gleam
{
    void AssetImporter::AddCustomImporter(const std::string_view extension, Type& type)
    {
        if (type.GetParent() == std::nullopt)
            type.SetParent(AssetImporterType);

        customImporters.emplace(std::string(extension), &type);
    }
    
    bool AssetImporter::HasImporter(const std::filesystem::path& assetPath)
    {
        std::string extension = assetPath.extension().string();
        return customImporters.contains(extension);
    }
    AssetBundle& AssetImporter::LoadImporter(const std::filesystem::path& assetPath)
    {
        std::filesystem::path importerPath = assetPath.string() + ".meta";
        if (exists(importerPath))
        {
            //加载已有导入器
            AssetBundle& assetBundle = AssetBundle::LoadJson(importerPath.string(), true);
            assetBundle.GetObject<AssetImporter>(0).assetPath = assetPath;
            return assetBundle;
        }

        //获取导入器类型
        std::string extension = std::filesystem::path(assetPath).extension().string();
        const Type& importerType = *customImporters.at(extension);
        //创建导入器
        AssetImporter* importer(static_cast<AssetImporter*>(importerType.Create()));
        importer->assetPath = assetPath;
        importer->assetBundleID = {}; //默认没有资源ID，需要导入时生成
        //存储导入器
        AssetBundle& assetBundle = AssetBundle::Create();
        assetBundle.AddAsset(importer, importerType, true);
        return assetBundle;
    }
    void AssetImporter::SaveImporter(AssetBundle& importer)
    {
        std::string importerPath = importer.GetObject<AssetImporter>(0).assetPath.string() + ".meta";
        AssetBundle::SaveJson(importerPath, importer);
    }
    void AssetImporter::UnLoadImporter(AssetBundle& importer)
    {
        AssetBundle::UnLoad(importer);
    }
    
    AssetBundle& AssetImporter::Import()
    {
        return ImportAsset(assetPath, assetBundleID);
    }
}