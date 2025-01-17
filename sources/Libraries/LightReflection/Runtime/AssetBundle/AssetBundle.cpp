// #include "AssetBundle.h"
//
// #include "../Serialization/JsonWriter.hpp"
//
// namespace Light
// {
//     void AssetBundle::Save(AssetBundle& assetBundle)
//     {
//         rapidjson::Document json = {};
//         json.Parse("{}");
//         JsonWriter writer = {json};
//         assetBundle.Transfer(writer);
//     }
//     void AssetBundle::Load(AssetBundle& assetBundle, DataTransferrer& serializer)
//     {
//     }
//
//     void AssetBundle::AddAsset(void* asset, Type* assetType)
//     {
//         assets.push_back(asset);
//         assetTypes.push_back(assetType);
//
//         int assetID = Random::Range(0, 999);
//         while (!assetIDsSet.emplace(assetID).second)
//             assetID = Random::Range(0, 999);
//         assetIDs.push_back(assetID);
//     }
//     void AssetBundle::RemoveAsset(void* asset)
//     {
//         auto it = std::ranges::find(assets, asset);
//         auto index = it - assets.begin();
//         assets.erase(assets.begin() + index);
//         assetTypes.erase(assetTypes.begin() + index);
//
//         auto assetIDIt = assetIDs.begin() + index;
//         assetIDsSet.erase(*assetIDIt);
//         assetIDs.erase(assetIDIt);
//     }
//     void AssetBundle::Transfer(DataTransferrer& serializer)
//     {
//     }
// }