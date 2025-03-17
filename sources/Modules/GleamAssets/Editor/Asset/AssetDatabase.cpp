#include "AssetDatabase.h"

#include "AssetImporter.h"
#include "GleamPersistence/Runtime/Resources.h"

namespace Gleam
{
    bool AssetDatabase::CanLoad(const std::filesystem::path& path)
    {
        return AssetImporter::CanImport(path);
    }
    bool AssetDatabase::HasLoaded(const std::filesystem::path& path)
    {
        auto assetMeta = AssetImporter::GetImporter(path);
        return AssetBundle::HasInMemory(assetMeta.GetAssetBundleID());
    }
    AssetBundle& AssetDatabase::Load(const std::filesystem::path& path)
    {
        AssetImporter& assetMeta = AssetImporter::GetImporter(path);
        AssetBundle& assetBundle = Resources::Load(assetMeta.GetAssetBundleID());

        return assetBundle;
    }
    void AssetDatabase::Reload(const std::filesystem::path& path)
    {
        AssetImporter& assetMeta = AssetImporter::GetImporter(path);
        AssetBundle& assetBundle = AssetBundle::GetAssetBundle(assetMeta.GetAssetBundleID());

        assetMeta.SaveAndReloadAsset();
        Resources::Save(assetBundle);
    }
    void AssetDatabase::Unload(const std::filesystem::path& path)
    {
        AssetImporter& assetMeta = AssetImporter::GetImporter(path);
        Resources::Unload(AssetBundle::GetAssetBundle(assetMeta.GetAssetBundleID()));
    }
    void AssetDatabase::Save(const std::filesystem::path& path)
    {
        AssetImporter& assetMeta = AssetImporter::GetImporter(path);
        AssetBundle& assetBundle = AssetBundle::GetAssetBundle(assetMeta.GetAssetBundleID());
        //保存到资源文件夹
        AssetBundle::SaveJson(path.string(), assetBundle);
        //保存到缓存文件夹
        Resources::Save(assetBundle);
    }
    void AssetDatabase::Save(const std::filesystem::path& path, AssetBundle& assetBundle)
    {
        //保存到资源文件夹
        AssetBundle::SaveJson(path.string(), assetBundle);
        //保存到缓存文件夹
        Resources::Save(assetBundle);
        //保存导入器
        AssetImporter::GetImporter(path).SaveAndReloadAsset();
    }
    void AssetDatabase::Move(const std::filesystem::path& oldPath, const std::filesystem::path& newPath)
    {
        if (!is_directory(oldPath))
            std::filesystem::rename(oldPath.string() + ".meta", newPath.string() + ".meta");
        std::filesystem::rename(oldPath, newPath);
    }
    void AssetDatabase::Delete(const std::filesystem::path& path)
    {
        if (is_directory(path))
        {
            remove_all(path);
        }
        else
        {
            std::filesystem::remove(path);
            std::filesystem::remove(path.string() + ".meta");
        }
    }

    uuids::uuid AssetDatabase::GetAssetBundleID(const std::filesystem::path& path)
    {
        return AssetImporter::GetImporter(path).GetAssetBundleID();
    }
    AssetBundle& AssetDatabase::GetAssetBundle(const std::filesystem::path& path)
    {
        return AssetBundle::GetAssetBundle(AssetImporter::GetImporter(path).GetAssetBundleID());
    }
    void AssetDatabase::Refresh(const std::filesystem::path& directory)
    {
        for (const auto& child : std::filesystem::directory_iterator{directory})
        {
            if (child.is_directory())
                Refresh(child.path());
            else if (AssetImporter::CanImport(child))
            {
                auto& assetImporter = AssetImporter::GetImporter(child);
                auto assetBundleID = assetImporter.GetAssetBundleID();
                if (assetBundleID.is_nil() || Resources::Has(assetBundleID) == false)
                {
                    assetBundleID = assetImporter.SaveAndReloadAsset();
                    AssetBundle& assetBundle = AssetBundle::GetAssetBundle(assetBundleID);
                    Resources::Save(assetBundle);
                    AssetBundle::Unload(assetBundle);
                }
            }
        }
    }
}