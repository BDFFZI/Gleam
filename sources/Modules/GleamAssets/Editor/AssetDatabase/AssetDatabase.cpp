#include "AssetDatabase.h"

#include "AssetImporter.h"
#include "GleamAssets/Editor/Configuration/SourceAssetDB.h"
#include "GleamPersistence/Runtime/Resources.h"
#include "GleamUtility/Runtime/File.h"

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

        AssetBundle::SetAutoClearPtrBuffer(false);

        if (AssetBundle::HasInMemory(assetMeta.GetAssetBundleID()))
        {
            AssetBundle assetBundle = assetMeta.SaveAndReadAsset();
            Resources::Save(assetBundle);
            Resources::Reload(assetBundle.GetID());
        }
        else
        {
            AssetBundle assetBundle = assetMeta.SaveAndReadAsset();
            Resources::Create(assetBundle);
        }

        AssetBundle::SetAutoClearPtrBuffer(true);
    }
    void AssetDatabase::Unload(const std::filesystem::path& path)
    {
        AssetImporter& assetMeta = AssetImporter::GetImporter(path);
        Resources::Unload(AssetBundle::GetAssetBundle(assetMeta.GetAssetBundleID()));
    }

    void AssetDatabase::CreateFolder(const std::filesystem::path& folderPath)
    {
        std::filesystem::create_directory(folderPath.string());
    }
    void AssetDatabase::Create(const std::filesystem::path& filePath, AssetBundle& assetBundle)
    {
        //保存到资源文件夹
        AssetBundle::SaveJson(filePath.string(), assetBundle);
        //保存到缓存文件夹
        Resources::Create(assetBundle);
        //保存导入器
        AssetImporter::GetImporter(filePath).SaveAndReadAsset();
    }
    void AssetDatabase::Save(const std::filesystem::path& filePath)
    {
        AssetImporter& assetMeta = AssetImporter::GetImporter(filePath);
        AssetBundle& assetBundle = AssetBundle::GetAssetBundle(assetMeta.GetAssetBundleID());
        //保存到资源文件夹
        AssetBundle::SaveJson(filePath.string(), assetBundle);
        //保存到缓存文件夹
        Resources::Save(assetBundle);
    }
    void AssetDatabase::Move(const std::filesystem::path& oldPath, const std::filesystem::path& newPath)
    {
        if (!is_directory(oldPath))
        {
            std::filesystem::path importer = oldPath.string() + ".meta";
            if (exists(importer)) std::filesystem::rename(importer, newPath.string() + ".meta");
        }
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
            std::filesystem::path importer = path.string() + ".meta";
            if (exists(importer)) std::filesystem::remove(path.string() + ".meta");
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
                //获取资源信息
                auto assetBundleID = assetImporter.GetAssetBundleID();
                auto& [timeStamp,contentHash] = SourceAssetDB::assetModificationInfos[child];
                //当导入器无效、资源缓存不存在、资源被修改时重新导入和更新资源信息
                if (Resources::Has(assetBundleID) == false || timeStamp != last_write_time(child).time_since_epoch().count())
                {
                    timeStamp = last_write_time(child).time_since_epoch().count();
                    contentHash = MD5(File::ReadAllText(child)).toArray();
                    Reload(child.path());
                }
            }
        }
    }
}