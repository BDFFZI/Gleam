#pragma once
#include <string>
#include <unordered_set>
#include <vector>

#include "../Type.hpp"
#include "LightEngine/Runtime/Utility/Random.h"

namespace Light
{
    struct AssetRef
    {
        std::string bundleID;
        std::string assetID;
    };

    class AssetBundle
    {
    public:
        static void Save(AssetBundle& assetBundle);
        static void Load(AssetBundle& assetBundle);

        void AddAsset(void* asset, Type* assetType);
        void RemoveAsset(void* asset);

    private:
        Light_MakeType_Friend

        std::vector<void*> assets;
        std::vector<Type*> assetTypes;
        std::vector<int> assetIDs;
        std::unordered_set<int> assetIDsSet;

        void Transfer(Serializer& serializer);
    };


    template <>
    struct SerializerTransfer<AssetBundle>
    {
        static void Invoke(Serializer& serializer, const char* name, TValue& value)
        {
        }
    };
}