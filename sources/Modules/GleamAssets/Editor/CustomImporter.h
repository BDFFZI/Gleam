#pragma once
#include "Asset/AssetImporter.h"

namespace Gleam
{
    class SceneImporter : public AssetImporter
    {
        AssetBundle& ImportAsset(const std::filesystem::path& assetPath, uuids::uuid& assetBundleID) override;
    };
}