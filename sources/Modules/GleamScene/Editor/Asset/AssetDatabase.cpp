#include "AssetDatabase.h"

#include "AssetMeta.h"

namespace Gleam
{
    bool AssetDatabase::CanLoad(const std::filesystem::path& path)
    {
        return AssetMeta::HasMeta(path);
    }
    bool AssetDatabase::HasLoaded(const std::filesystem::path& path)
    {
        auto assetMeta = AssetMeta::GetMeta(path);
        return AssetBundle::HasInMemory(assetMeta->GetAssetBundleID());
    }
    AssetBundle& AssetDatabase::Load(const std::filesystem::path& path, const bool reload)
    {
        std::unique_ptr<AssetMeta> assetMeta = AssetMeta::GetMeta(path);

        AssetBundle* assetBundle = nullptr;
        if (!reload)
        {
            if (AssetBundle::HasInMemory(assetMeta->GetAssetBundleID()))
                assetBundle = &AssetBundle::GetAssetBundle(assetMeta->GetAssetBundleID());
            else
            {
                auto cachePath = cacheDirectory / to_string(assetMeta->GetAssetBundleID());
                if (!reload && exists(cachePath)) //优先加载缓存的二进制数据，以加速大文件的加载
                    assetBundle = &AssetBundle::LoadBinary(cachePath.string(), true);
            }
        }

        if (assetBundle == nullptr) //重载或首次导入
        {
            //导入资源
            assetMeta->SaveAndReloadAsset();
            assetBundle = &AssetBundle::GetAssetBundle(assetMeta->GetAssetBundleID());
            //缓存资源
            auto cachePath = cacheDirectory / to_string(assetBundle->GetID());
            AssetBundle::SaveBinary(cachePath.string(), *assetBundle);
            AssetBundle::SaveMeta(cachePath.string(), *assetBundle);
        }

        return *assetBundle;
    }
    void AssetDatabase::UnLoad(const std::filesystem::path& path)
    {
        std::unique_ptr<AssetMeta> assetMeta = AssetMeta::GetMeta(path);
        assetMeta->UnLoadAsset();
    }
    void AssetDatabase::Save(const std::filesystem::path& path)
    {
        //保存到资源文件夹
        std::unique_ptr<AssetMeta> assetMeta = AssetMeta::GetMeta(path);
        assetMeta->SaveAsset();
        //保存到缓存文件夹
        AssetBundle& assetBundle = AssetBundle::GetAssetBundle(assetMeta->GetAssetBundleID());
        auto cachePath = cacheDirectory / to_string(assetBundle.GetID());
        AssetBundle::SaveBinary(cachePath.string(), assetBundle);
        AssetBundle::SaveMeta(cachePath.string(), assetBundle);
    }
    uuids::uuid AssetDatabase::GetAssetBundleID(const std::filesystem::path& path)
    {
        return AssetMeta::GetMeta(path)->GetAssetBundleID();
    }
}