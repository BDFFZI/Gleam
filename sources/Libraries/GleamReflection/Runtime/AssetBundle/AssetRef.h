#pragma once
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    struct AssetRef
    {
        uuids::uuid assetBundleID;
        int assetID;

        bool operator==(const AssetRef& other) const;
    };

    Gleam_MakeType(AssetRef, "")
    {
        Gleam_MakeType_AddField(assetBundleID);
        Gleam_MakeType_AddField(assetID);
    }
}

template <>
struct std::hash<Gleam::AssetRef>
{
    size_t operator()(const Gleam::AssetRef& value) const noexcept;
};