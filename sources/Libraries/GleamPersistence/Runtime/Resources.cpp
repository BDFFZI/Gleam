#include "Resources.h"

namespace Gleam
{
    bool Resources::Has(const uuids::uuid assetBundleID)
    {
        return exists(resourceDirectory / to_string(assetBundleID));
    }
    AssetBundle& Resources::Load(const uuids::uuid assetBundleID)
    {
        auto assetBundlePath = resourceDirectory / to_string(assetBundleID);
        AssetBundleMeta assetBundleMeta = AssetBundle::LoadMeta(assetBundlePath.string());
        for (auto dependencyID : assetBundleMeta.dependencies)
            Load(dependencyID);

        assetBundleRefCount[assetBundleID]++;

        if (AssetBundle::HasInMemory(assetBundleID))
            return AssetBundle::GetAssetBundle(assetBundleID);
        return AssetBundle::LoadBinary(assetBundlePath.string());
    }
    void Resources::Unload(const uuids::uuid assetBundleID)
    {
        auto assetBundlePath = resourceDirectory / to_string(assetBundleID);
        AssetBundleMeta assetBundleMeta = AssetBundle::LoadMeta(assetBundlePath.string());
        for (auto dependencyID : assetBundleMeta.dependencies)
            Unload(dependencyID);

        assetBundleRefCount[assetBundleID]--;

        if (assetBundleRefCount[assetBundleID] == 0)
            AssetBundle::UnLoad(AssetBundle::GetAssetBundle(assetBundleID));
    }

    void Resources::Reload(const uuids::uuid assetBundleID)
    {
        assert(assetBundleRefCount[assetBundleID] > 0 && "资源包未被加载！");

        auto assetBundlePath = resourceDirectory / to_string(assetBundleID);
        AssetBundle::LoadBinary(assetBundlePath.string(), true);
    }
    void Resources::Save(AssetBundle& assetBundle)
    {
        auto assetBundlePath = resourceDirectory / to_string(assetBundle.GetID());
        AssetBundle::SaveBinary(assetBundlePath.string(), assetBundle);
        AssetBundle::SaveMeta(assetBundlePath.string(), assetBundle);
    }
}