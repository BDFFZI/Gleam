// #pragma once
// #include <string>
// #include <unordered_set>
// #include <vector>
// #include <stduuid/uuid.h>
//
// #include "../Type.h"
// #include "GleamMath/Runtime/Random.h"
//
// namespace Gleam
// {
//     struct AssetRef
//     {
//         std::string bundleID;
//         std::string assetID;
//     };
//
//     class AssetBundle
//     {
//     public:
//         static void Save(AssetBundle& assetBundle);
//         static 
//         static void Load(AssetBundle& assetBundle);
//
//         void AddAsset(void* asset, Type* assetType);
//         void RemoveAsset(void* asset);
//
//     private:
//         Gleam_MakeType_Friend
//
//         static std::unordered_map<uuids::uuid,std::>
//
//         uuids::uuid bundleID;
//         std::vector<void*> assets;
//         std::vector<Type*> assetTypes;
//         std::vector<int> assetIDs;
//         std::unordered_set<int> assetIDsSet;
//
//         void Transfer(DataTransferrer& serializer);
//     };
//
//
//     template <>
//     struct SerializerTransfer<AssetBundle>
//     {
//         static void Invoke(DataTransferrer& serializer, const char* name, TValue& value)
//         {
//         }
//     };
// }