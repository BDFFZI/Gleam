#pragma once
#include <filesystem>

#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"

namespace Gleam
{
    class AssetDatabase
    {
    public:
        static bool CanLoad(const std::filesystem::path& path);
        static bool HasLoaded(const std::filesystem::path& path);
        static AssetBundle& Load(const std::filesystem::path& path);
        static void Reload(const std::filesystem::path& path);
        static void Unload(const std::filesystem::path& path);

        static void CreateFolder(const std::filesystem::path& folderPath);
        static void Create(const std::filesystem::path& filePath, AssetBundle& assetBundle);
        static void Save(const std::filesystem::path& filePath);
        static void Move(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);
        static void Delete(const std::filesystem::path& path);

        static uuids::uuid GetAssetBundleID(const std::filesystem::path& path);
        static AssetBundle& GetAssetBundle(const std::filesystem::path& path);

        /**
         * 将目标目录中的所有资源解压到Resources以便使用依赖加载和引用计数功能
         * @param directory 
         */
        static void Refresh(const std::filesystem::path& directory);
    };
}