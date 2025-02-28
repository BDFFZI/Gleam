#pragma once
#include <string_view>
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
        static void Destroy(AssetBundle& assetBundle, bool retainAssets = false);
        static bool Exists(uuids::uuid assetBundleID);
        static void Save(AssetBundle& assetBundle);
        static AssetBundle& Load(uuids::uuid assetBundleID);

        static uuids::uuid GetIDFromJson(std::string_view fileName);
        static void SaveJson(std::string_view fileName, AssetBundle& assetBundle, bool saveBinary = true);
        static AssetBundle& LoadJson(std::string_view fileName);
        static void DumpJson(std::string_view fileName);
        static std::optional<void*> Load(AssetRef assetRef);

        void AddAsset(void* data, Type& dataType);
        void RemoveAsset(void* data);
        void ClearAssets();
        uuids::uuid GetID() const;
        const std::vector<Asset>& GetAssets();

        AssetBundle() = default;
        AssetBundle(AssetBundle&&) = default;
        AssetBundle& operator=(AssetBundle&&) = default;

    private:
        template <typename T>
        friend struct DataTransferrer_Transfer;

        Gleam_MakeType_Friend

        inline static std::string assetBundleDirectory = "./Library/";
        inline static std::unordered_map<uuids::uuid, std::unique_ptr<AssetBundle>> assetBundles = {};
        inline static std::unordered_map<void*, AssetRef> dataToAsset = {}; //数据对应的资源
        inline static std::unordered_map<AssetRef, void*> assetToData = {}; //资源对应的数据
        inline static std::unordered_map<void*, AssetRef> pointerMapping = {}; //所有指针绑定的资源引用信息

        uuids::uuid id;
        std::vector<Asset> assets;
        std::unordered_set<int> assetIDSet;

        void RefreshSet();

        ///发现一个逆天c++bug
        ///若一个类中同时有std::vector和std::unordered_set，且std::vector的元素是仅可移动的类型，
        ///那么这个类作为std::vector使用时，即使添加右值元素也无法编译。
        ///解决方法是必须显式写出移动函数，隐式生成的有问题。
    };

    Gleam_MakeType(AssetBundle, "")
    {
        Gleam_MakeType_AddField(id);
        Gleam_MakeType_AddField(assets);
    }

    //将指针类型转为资源引用传输
    template <typename TValue>
    struct DataTransferrer_Transfer<TValue*>
    {
        static void Invoke(DataTransferrer& serializer, TValue*& value)
        {
            AssetRef assetRef = AssetBundle::pointerMapping[&value];
            {
                if (AssetBundle::dataToAsset.contains(value))
                    assetRef = AssetBundle::dataToAsset.at(value); //根据数据获取资源引用
                serializer.Transfer(assetRef);
                if (!assetRef.assetBundleID.is_nil() && !AssetBundle::assetToData.contains(assetRef))
                    value = static_cast<TValue*>(AssetBundle::Load(assetRef).value_or(nullptr)); //根据资源引用获取数据
            }
            AssetBundle::pointerMapping[&value] = assetRef;
        }
    };
}