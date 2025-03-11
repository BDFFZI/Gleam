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
        static AssetBundle& Load(const std::filesystem::path& path, bool reload = false);
        static void UnLoad(const std::filesystem::path& path);
        static void Save(const std::filesystem::path& path);

        static uuids::uuid GetAssetBundleID(const std::filesystem::path& path);

    private:
        inline static std::filesystem::path assetDirectory = "Assets";
        inline static std::filesystem::path cacheDirectory = "Library";
    };
}