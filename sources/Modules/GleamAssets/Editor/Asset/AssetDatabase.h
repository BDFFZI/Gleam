#pragma once
#include <filesystem>

#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"

namespace Gleam
{
    class AssetDatabase
    {
    public:
        static bool CanLoad(const std::filesystem::path& path);
        static void ReLoad(const std::filesystem::path& path);
        static AssetBundle& Load(const std::filesystem::path& path);
        static void Save(const std::filesystem::path& path, AssetBundle& assetBundle);
    };
}