#pragma once
#include <unordered_set>

#include "Asset.h"
#include "AssetRef.h"
#include "GleamReflection/Runtime/Type.h"
#include "GleamPersistence/Runtime/Serializer/JsonWriter.h"

namespace Gleam
{
    struct AssetBundleMeta
    {
        std::vector<uuids::uuid> dependencies;
    };
    Gleam_MakeType(AssetBundleMeta, "")
    {
        Gleam_MakeType_AddField(dependencies);
    }

    class AssetBundle
    {
    public:
        static AssetBundle& Create(uuids::uuid assetBundleID = {});
        static void Save(AssetBundle& assetBundle);
        static void SaveMeta(AssetBundle& assetBundle);
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

        const std::vector<Asset>& GetAssets() const;
        uuids::uuid GetID() const;
        const Asset& GetAsset(int index) const;
        std::optional<std::reference_wrapper<const Asset>> GetAssetFromID(int assetID) const;
        template <typename T>
        T& GetData(const int index)
        {
            return *static_cast<T*>(assets[index].GetDataRef());
        }

        void AddAsset(void* data, const Type& dataType);
        template <class T> requires !std::is_reference_v<T>
        Asset& AddAsset(T&& data)
        {
            Asset asset = {GenerateAssetID(), std::move(data)};
            return EmplaceAsset(std::move(asset));
        }
        void RemoveAsset(void* data);

        void ClearAssets();
        Asset& EmplaceAsset(Asset&& asset);
        Asset ExtractAsset(int assetID);

        AssetBundle() = default;
        AssetBundle(AssetBundle&&) = default;
        AssetBundle& operator=(AssetBundle&&) = default;

    private:
        template <typename T>
        friend struct FieldDataTransferrer_Transfer;

        Gleam_MakeType_Friend

        inline static std::string assetBundleDirectory = "./Library/";
        inline static std::unordered_map<uuids::uuid, AssetBundle> assetBundles = {};
        inline static std::unordered_map<void*, AssetRef> dataToAsset = {}; //数据对应的资源
        inline static std::unordered_map<AssetRef, void*> assetToData = {}; //资源对应的数据
        /**
         * 缓存指针绑定的资源引用。
         * 
         * 由于首次序列化指针时，所有资源都未加载完成，因此无法通过资源引用获取资源。
         * 而待资源加载完毕后重新链接指针时，由于不再从文件中读取信息，因此无法获取指针的引用资源。
         * 故需要在第一次序列化时缓存指针的资源引用，然后第二次序列化时提取出来。
         */
        inline static std::unordered_map<void*, AssetRef> pointerMapping = {};

        uuids::uuid id;
        std::vector<Asset> assets;
        std::unordered_set<int> assetIDSet;

        void BuildAssetIndex();
        int GenerateAssetID() const;

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