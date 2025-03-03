#pragma once
#include <unordered_set>
#include <uuid.h>

#include "Asset.h"
#include "AssetRef.h"
#include "GleamReflection/Runtime/Type.h"
#include "GleamReflection/Runtime/Serialization/JsonWriter.h"

namespace Gleam
{
    class AssetBundle
    {
    public:
        static AssetBundle& Create(uuids::uuid assetBundleID = {});
        static void Save(AssetBundle& assetBundle);
        static AssetBundle& Load(AssetBundle& newAssetBundle, bool reload = false);
        static AssetBundle& Load(uuids::uuid assetBundleID, bool reload = false);
        static void UnLoad(AssetBundle& assetBundle, bool retainAssets = false);

        static std::optional<AssetRef> GetAssetRef(void* data);
        static std::optional<void*> GetDataRef(const AssetRef& assetRef);
        static AssetBundle& GetAssetBundle(uuids::uuid assetBundleID);
        static bool HasInDisk(uuids::uuid assetBundleID);
        static bool HasInMemory(uuids::uuid assetBundleID);

        static uuids::uuid GetIDFromJson(std::string_view fileName);
        static void SaveJson(std::string_view fileName, AssetBundle& assetBundle, bool saveBinary = true);
        static AssetBundle& LoadJson(std::string_view fileName, bool reload = false);
        static void DumpJson(std::string_view fileName);

        uuids::uuid GetID() const;
        const std::vector<Asset>& GetAssets() const;
        std::optional<std::reference_wrapper<const Asset>> GetAsset(int assetID) const;
        template <typename T>
        T& GetData(const int index)
        {
            return *static_cast<T*>(assets[index].GetDataRef());
        }

        void AddAsset(void* data, const Type& dataType);
        void RemoveAsset(void* data);
        void ClearAssets();
        void EmplaceAsset(Asset&& asset);
        Asset ExtractAsset(int assetID);

        AssetBundle() = default;
        AssetBundle(AssetBundle&&) = default;
        AssetBundle& operator=(AssetBundle&&) = default;

    private:
        template <typename T>
        friend struct DataTransferrer_Transfer;

        Gleam_MakeType_Friend

        inline static std::string assetBundleDirectory = "./Library/";
        inline static std::unordered_map<uuids::uuid, AssetBundle> assetBundles = {};
        inline static std::unordered_map<void*, AssetRef> dataToAsset = {}; //数据对应的资源
        inline static std::unordered_map<AssetRef, void*> assetToData = {}; //资源对应的数据
        inline static std::unordered_map<void*, AssetRef> pointerMapping = {}; //所有指针绑定的资源引用信息

        uuids::uuid id;
        std::vector<Asset> assets;
        std::unordered_set<int> assetIDSet;

        void BuildAssetIndex();

        ///发现一个离谱c++bug
        ///若一个类中同时有std::vector和std::unordered_set，且std::vector的元素是仅可移动的类型，
        ///那么这个类作为std::vector使用时，即使添加右值元素也无法编译。
        ///解决方法是必须显式写出移动函数，隐式生成的有问题。
    };

    Gleam_MakeType(AssetBundle, "4139AE7B-E48E-4A20-8CB3-FA814D8942EF")
    {
        Gleam_MakeType_AddField(id);
        Gleam_MakeType_AddField(assets);
    }
}

// 依赖自定义指针传输来同步资源引用
// ReSharper disable once CppUnusedIncludeDirective
#include "PointerTransferrer.h"