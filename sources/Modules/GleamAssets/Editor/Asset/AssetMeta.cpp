#include "AssetMeta.h"

#include "GleamPersistence/Runtime/JsonUtility.h"
#include "GleamUtility/Runtime/File.h"

namespace Gleam
{
    void AssetMeta::AddCustomMeta(const std::string_view extension, Type& type)
    {
        if (type.GetParent() == std::nullopt)
            type.SetParent(AssetMetaType);

        customMetas.emplace(std::string(extension), &type);
    }

    bool AssetMeta::HasMeta(const std::filesystem::path& assetPath)
    {
        std::string extension = assetPath.extension().string();
        return customMetas.contains(extension);
    }
    std::unique_ptr<AssetMeta> AssetMeta::GetMeta(const std::filesystem::path& assetPath)
    {
        //获取资源元数据的基本信息
        std::string extension = std::filesystem::path(assetPath).extension().string();
        const Type& metaType = *customMetas.at(extension);
        std::filesystem::path metaPath = assetPath.string() + ".meta";
        //加载资源元数据
        std::unique_ptr<AssetMeta> assetMeta(static_cast<AssetMeta*>(metaType.Create()));
        if (exists(metaPath)) //加载已有的资源元数据
            JsonUtility::FromJson(File::ReadAllText(metaPath.string()), metaType, assetMeta.get());
        assetMeta->assetPath = assetPath;

        return assetMeta;
    }
    void AssetMeta::SaveAndReloadAsset()
    {
        //重新载入资源
        LoadAsset(assetBundleID);
        //保存元信息
        File::WriteAllText(assetPath.string() + ".meta", JsonUtility::ToJson(this, AssetMetaType, true));
    }
    void AssetMeta::UnLoadAsset()
    {
        AssetBundle::UnLoad(AssetBundle::GetAssetBundle(assetBundleID));
    }
    void AssetMeta::SaveAsset()
    {
        AssetBundle::SaveJson(assetPath.string(), AssetBundle::GetAssetBundle(assetBundleID));
    }
}