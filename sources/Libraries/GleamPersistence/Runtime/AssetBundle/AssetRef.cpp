#include "AssetRef.h"

bool Gleam::AssetRef::operator==(const AssetRef& other) const
{
    return assetBundleID == other.assetBundleID
        && assetID == other.assetID;
}

size_t std::hash<Gleam::AssetRef>::operator()(const Gleam::AssetRef& value) const noexcept
{
    std::size_t seed = 0x4A5234EC;
    seed ^= (seed << 6) + (seed >> 2) + 0x548E2F63 + std::hash<uuids::uuid>()(value.assetBundleID);
    seed ^= (seed << 6) + (seed >> 2) + 0x6CE9ED95 + static_cast<std::size_t>(value.assetID);
    return seed;
}