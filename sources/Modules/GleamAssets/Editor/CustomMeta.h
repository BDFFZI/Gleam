#pragma once
#include "Asset/AssetMeta.h"

namespace Gleam
{
    class SceneImporter : public AssetMeta
    {
        void LoadAsset(uuids::uuid& assetBundleID) override;
        void SaveAsset() override;
    };
}