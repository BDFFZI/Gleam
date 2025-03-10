#include "AssetDatabase.h"

#include "AssetImporter.h"

namespace Gleam
{
    bool AssetDatabase::CanLoad(const std::filesystem::path& path)
    {
        return AssetImporter::HasImporter(path);
    }
    bool AssetDatabase::HasLoaded(const std::filesystem::path& path)
    {
        AssetBundle& assetImporterBundle = AssetImporter::LoadImporter(path);
        AssetImporter& assetImporter = assetImporterBundle.GetObject<AssetImporter>(0);
        return AssetBundle::HasInMemory(assetImporter.assetBundleID);
    }
    AssetBundle& AssetDatabase::Load(const std::filesystem::path& path, const bool reload)
    {
        AssetBundle& assetImporterBundle = AssetImporter::LoadImporter(path);
        AssetImporter& assetImporter = assetImporterBundle.GetObject<AssetImporter>(0);

        AssetBundle* assetBundle = nullptr;
        if (!reload)
        {
            if (AssetBundle::HasInMemory(assetImporter.assetBundleID))
                assetBundle = &AssetBundle::GetAssetBundle(assetImporter.assetBundleID);
            else
            {
                auto cachePath = cacheDirectory / to_string(assetImporter.assetBundleID);
                if (!reload && exists(cachePath)) //优先加载缓存的二进制数据，以加速大文件的加载
                    assetBundle = &AssetBundle::LoadBinary(cachePath.string(), true);
            }
        }

        if (assetBundle == nullptr) //重载或首次导入
        {
            //导入资源    
            assetBundle = &assetImporter.Import();
            //缓存资源
            auto cachePath = cacheDirectory / to_string(assetBundle->GetID());
            AssetBundle::SaveBinary(cachePath.string(), *assetBundle);
            AssetBundle::SaveMeta(cachePath.string(), *assetBundle);
            //保存导入器
            AssetImporter::SaveImporter(assetImporterBundle);
        }

        AssetImporter::UnLoadImporter(assetImporterBundle);
        return *assetBundle;
    }
    void AssetDatabase::Save(const std::filesystem::path& path, AssetBundle& assetBundle)
    {
        //保存到资源文件夹
        AssetBundle::SaveJson(path.string(), assetBundle);
        //保存到缓存文件夹
        auto cachePath = cacheDirectory / to_string(assetBundle.GetID());
        AssetBundle::SaveBinary(cachePath.string(), assetBundle);
        AssetBundle::SaveMeta(cachePath.string(), assetBundle);
    }
}