#include "AssetDatabase.h"

#include "AssetImporter.h"
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
        AssetBundle& assetBundle = AssetBundle::GetAssetBundle(assetMeta.GetAssetBundleID());

        assetMeta.SaveAndReloadAsset();
        Resources::Save(assetBundle);
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
        AssetImporter::GetImporter(filePath).SaveAndReloadAsset();
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

                bool needReimport = false;
                //验证导入器或导入缓存是否存在
                auto assetBundleID = assetImporter.GetAssetBundleID();
                if (assetBundleID.is_nil() || Resources::Has(assetBundleID) == false)
                    needReimport = true;
                else
                {
                    //验证文件写入时间戳是否一致
                    int64_t currentTimeStamp = last_write_time(child).time_since_epoch().count();
                    if (currentTimeStamp != assetImporter.assetTimeStamp)
                    {
                        assetImporter.assetTimeStamp = currentTimeStamp;
                        //验证文件内容是否一致
                        std::string content = File::ReadAllText(child);
                        uuids::uuid contentHash = MD5(content).toArray();
                        if (contentHash != assetImporter.assetContentHash)
                        {
                            assetImporter.assetContentHash = contentHash;
                            needReimport = true;
                        }
                        else
                            assetImporter.Save();
                    }
                }

                if (needReimport)
                {
                    assetBundleID = assetImporter.SaveAndReloadAsset();
                    AssetBundle& assetBundle = AssetBundle::GetAssetBundle(assetBundleID);
                    Resources::Create(assetBundle);
                    AssetBundle::Unload(assetBundle);
                }
            }
        }
    }
}