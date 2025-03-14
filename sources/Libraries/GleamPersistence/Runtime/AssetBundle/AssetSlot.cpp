#include "AssetSlot.h"

namespace Gleam
{
    AssetSlot::AssetSlot(const int id, Asset&& asset)
    : id(id), asset(std::move(asset))
    {
    }
    int AssetSlot::GetID() const
    {
        return id;
    }
    const Asset& AssetSlot::GetAsset() const
    {
        return asset;
    }
    Asset& AssetSlot::GetAsset()
    {
        return asset;
    }
}