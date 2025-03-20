#pragma once
#include <unordered_set>

#include "Asset.h"
#include "AssetRef.h"
#include "AssetSlot.h"
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

    /**
     * 一个功能完整的资源持久化容器。
     *
     * 支持同时存储多个资源，并且能存储资源间的依赖信息和自动化资源添加，可以将运行时对象的信息完整持久化。
     */
    class AssetBundle
    {
    public:
        static auto GetAllAssetBundles()
        {
            return assetBundles | std::views::values;
        }

        static AssetBundle& Create(uuids::uuid assetBundleID = {});
        static void Unload(AssetBundle& assetBundle, bool releaseOwnership = false);

        static AssetBundle& Load(AssetBundle& newAssetBundle, bool reload = false);
        static AssetBundle& LoadBinary(std::string_view fileName, bool reload = false);
        static AssetBundle& LoadJson(std::string_view fileName, bool reload = false);
        static AssetBundleMeta LoadMeta(std::string_view fileName);

        static void SaveBinary(std::string_view fileName, AssetBundle& assetBundle);
        static void SaveJson(std::string_view fileName, AssetBundle& assetBundle);
        static void SaveMeta(std::string_view fileName, AssetBundle& assetBundle);
        static void DumpJsonToBinary(std::string_view jsonFile, std::string_view binaryFile, bool saveMeta);

        static std::optional<AssetRef> GetAssetRef(void* data);
        static std::optional<void*> GetObject(const AssetRef& assetRef);
        static AssetBundle& GetAssetBundle(uuids::uuid assetBundleID);
        static bool HasInMemory(uuids::uuid assetBundleID);

        static uuids::uuid GetIDFromJson(std::string_view fileName);

        AssetBundle() = default;
        AssetBundle(AssetBundle&&) = default;
        AssetBundle& operator=(AssetBundle&&) = default;

        uuids::uuid GetID() const;
        const std::vector<AssetSlot>& GetAssetSlots() const;
        std::optional<std::reference_wrapper<AssetSlot>> GetAssetSlot(void* object);
        std::optional<std::reference_wrapper<AssetSlot>> GetAssetSlot(int slotID);
        Asset& GetAsset(int index);
        template <typename T>
        T& GetObject(const int index)
        {
            return *static_cast<T*>(assetSlots[index].GetAsset().GetObject());
        }

        void AddAsset(Asset&& asset);
        template <class T> requires !std::is_reference_v<T>
        Asset& AddAsset(T&& data)
        {
            Asset asset = Asset{std::move(data)};
            return EmplaceAsset(std::move(asset));
        }
        void RemoveAsset(void* data);

        void ClearAssets(bool releaseOwnership = false);
        Asset ExtractAsset(int slotID);
        Asset& EmplaceAsset(Asset&& asset, std::optional<int> expectedSlotID = std::nullopt);

    private:
        template <typename T>
        friend struct FieldDataTransferrer_Transfer;

        Gleam_MakeType_Friend

        inline static std::unordered_map<uuids::uuid, AssetBundle> assetBundles = {};
        inline static std::unordered_map<void*, AssetRef> objectToAssetSlot = {}; //数据对应的资源
        inline static std::unordered_map<AssetRef, void*> assetSlotToObject = {}; //资源对应的数据
        /**
         * 每个指针字段绑定的资源引用。
         * 
         * 由于首次序列化指针时，所有资源都未加载完成，因此无法通过资源引用获取资源。
         * 而待资源加载完毕后重新链接指针时，由于不再从文件中读取信息，因此无法获取指针的引用资源。
         * 故需要在第一次序列化时缓存指针的资源引用，然后第二次序列化时提取出来。
         *
         * 默认情况下该表在每次加载完资源包后清空，但若不清空，则可以实现资源包加载时丢失引用但保存时依然能恢复引用的功能。
         * 但那样做会导致几个问题：
         * 1. 不同时间创建的对象可以使用相同的指针地址（尤其是实体的内存布局，很容易触发），因此该表存储的值可能有误。
         * 2. 指针为空除了因为引用丢失，也可能是用户有意设置，但因为无法区分，用户将始终无法使指针在持久化时置空。
         */
        inline static std::unordered_map<std::uintptr_t, AssetRef> pointerToAssetSlot = {};

        uuids::uuid id;
        std::vector<AssetSlot> assetSlots;
        std::unordered_set<int> assetSlotIDSet;

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
        Gleam_MakeType_AddField(assetSlots);
    }
}

// 引导编译器编译资源包功能所需的模板特化
// ReSharper disable once CppUnusedIncludeDirective
#include "AssetBundleTransferrer.h"