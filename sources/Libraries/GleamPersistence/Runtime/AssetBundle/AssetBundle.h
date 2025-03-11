#pragma once
#include <unordered_set>

#include "Asset.h"
#include "AssetRef.h"
#include "GleamReflection/Runtime/Type.h"
#include "GleamPersistence/Runtime/Serializer/JsonWriter.h"

#undef GetObject

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
        static void Clear();
        static auto GetAllAssetBundles()
        {
            return assetBundles | std::views::values;
        }

        static void SaveBinary(std::string_view fileName, AssetBundle& assetBundle);
        static void SaveJson(std::string_view fileName, AssetBundle& assetBundle);
        static void SaveMeta(std::string_view fileName, AssetBundle& assetBundle);
        static void DumpJsonToBinary(std::string_view jsonFile, std::string_view binaryFile, bool saveMeta);

        static AssetBundle& Load(AssetBundle& newAssetBundle, bool reload = false);
        static AssetBundle& LoadBinary(std::string_view fileName, bool reload = false);
        static AssetBundle& LoadJson(std::string_view fileName, bool reload = false);
        static AssetBundleMeta LoadMeta(std::string_view fileName);
        static void UnLoad(AssetBundle& assetBundle, bool retainAssets = false);

        static std::optional<AssetRef> GetAssetRef(void* data);
        static std::optional<void*> GetObject(const AssetRef& assetRef);
        static AssetBundle& GetAssetBundle(uuids::uuid assetBundleID);
        static bool HasInMemory(uuids::uuid assetBundleID);

        static uuids::uuid GetIDFromJson(std::string_view fileName);

        AssetBundle() = default;
        AssetBundle(AssetBundle&&) = default;
        AssetBundle& operator=(AssetBundle&&) = default;

        const std::vector<Asset>& GetAssets() const;
        uuids::uuid GetID() const;
        const Asset& GetAsset(int index) const;
        std::optional<std::reference_wrapper<const Asset>> GetAsset(void* data);
        std::optional<std::reference_wrapper<const Asset>> GetAssetFromID(int assetID) const;
        template <typename T>
        T& GetObject(const int index)
        {
            return *static_cast<T*>(assets[index].GetObject());
        }

        void AddAsset(void* data, const Type& dataType, bool ownership);
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

    private:
        template <typename T>
        friend struct FieldDataTransferrer_Transfer;

        Gleam_MakeType_Friend

        inline static std::unordered_map<uuids::uuid, AssetBundle> assetBundles = {};
        inline static std::unordered_map<void*, AssetRef> dataToAsset = {}; //数据对应的资源
        inline static std::unordered_map<AssetRef, void*> assetToData = {}; //资源对应的数据
        /**
         * 每个指针字段绑定的资源引用。
         * 
         * 由于首次序列化指针时，所有资源都未加载完成，因此无法通过资源引用获取资源。
         * 而待资源加载完毕后重新链接指针时，由于不再从文件中读取信息，因此无法获取指针的引用资源。
         * 故需要在第一次序列化时缓存指针的资源引用，然后第二次序列化时提取出来。
         */
        inline static std::unordered_map<std::uintptr_t, AssetRef> pointerMapping = {};

        static void SerializePtr(FieldDataTransferrer& serializer, void*& value, std::type_index);

        uuids::uuid id;
        std::vector<Asset> assets;
        std::unordered_set<int> assetIDSet;

        void BuildAssetIndex();
        int GenerateAssetID() const;
        void AddAssetDependency();

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