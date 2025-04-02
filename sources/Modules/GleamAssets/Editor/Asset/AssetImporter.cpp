#include "AssetImporter.h"

#include "GleamPersistence/Runtime/JsonUtility.h"
#include "GleamUtility/Runtime/File.h"

namespace Gleam
{
    void AssetImporter::MakeAssetImporter(const std::string_view extension, Type& type)
    {
        if (type.GetParent() == std::nullopt)
            type.SetParent(AssetImporterType);

        customImporters.emplace(std::string(extension), &type);
    }
    void AssetImporter::MakeAssetImporter(const std::string_view extension, const Type& type)
    {
        customImporters.emplace(std::string(extension), &type);
    }

    bool AssetImporter::CanImport(const std::filesystem::path& assetPath)
    {
        std::string extension = assetPath.extension().string();
        return customImporters.contains(extension);
    }
    AssetImporter& AssetImporter::GetImporter(const std::filesystem::path& assetPath, const bool reload)
    {
        if (reload == false && cacheImporters.contains(assetPath))
            return *cacheImporters.at(assetPath);

        //获取资源导入器基本信息
        std::string extension = std::filesystem::path(assetPath).extension().string();
        const Type& metaType = *customImporters.at(extension);
        std::filesystem::path metaPath = assetPath.string() + ".meta";
        //创建资源导入器
        std::unique_ptr<AssetImporter> assetMeta(static_cast<AssetImporter*>(metaType.Create()));
        if (exists(metaPath)) //加载已有的资源导入器数据
            JsonUtility::FromJson(File::ReadAllText(metaPath.string()), metaType, assetMeta.get());
        assetMeta->assetPath = assetPath;

        return *cacheImporters.emplace(assetPath, std::move(assetMeta)).first->second;
    }
    void AssetImporter::Save()
    {
        //保存元信息
        File::WriteAllText(assetPath.string() + ".meta", JsonUtility::ToJson(this, AssetImporterType, true));
    }
    AssetBundle AssetImporter::SaveAndReadAsset()
    {
        //重新读取资源内容
        AssetBundle assetBundle = ReadAsset(assetPath, assetBundleID);
        assetBundleID = assetBundle.GetID();
        //保存元信息
        Save();

        return assetBundle;
    }
}