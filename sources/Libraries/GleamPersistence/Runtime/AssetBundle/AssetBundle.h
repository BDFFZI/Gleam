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

    /**
     * 统计资源包中AssetRef的使用情况，可以此计算资源包依赖
     */
    class AssetRefStatistician : public FieldDataTransferrer
    {
    public:
        std::vector<AssetRef> result;

        void FallbackTransfer(void* value, const std::type_index typeIndex) override
        {
            if (typeIndex == typeid(AssetRef))
            {
                AssetRef& assetRef = *static_cast<AssetRef*>(value);
                if (!assetRef.assetBundleID.is_nil())
                    result.push_back(assetRef);
                return;
            }

            FieldDataTransferrer::FallbackTransfer(value, typeIndex);
        }
    };

    /**
     * 统计资源包中对象的指针引用的未托管对象，可以此计算要额外自动添加到资源包中的对象。
     *
     * 具体的统计函数由FieldDataTransferrer_Transfer实现
     */
    class ObjectRefStatistician : public FieldDataTransferrer
    {
    public:
        std::vector<Asset> dependencies; //指针引用的未托管可持久化对象
        std::unordered_map<void*, std::vector<void**>> dependencyUsers; //引用这些对象的指针
    };

    /**
     * 一个不会进行实际传输的序列化器，仅用于触发指针序列化事件，以解决因序列化顺序导致指针无法引用到对象的问题
     */
    class PointerSerializer : public FieldDataTransferrer
    {
    };

    /**
     * 针对对象依赖（指针）的特制传输器
     * @tparam TValue 
     */
    template <typename TValue>
    struct FieldDataTransferrer_Transfer<TValue*>
    {
        static void Invoke(FieldDataTransferrer& serializer, TValue*& value)
        {
            //将指针视作AssetRef进行传输
            if (
                dynamic_cast<Serializer*>(&serializer) ||
                dynamic_cast<PointerSerializer*>(&serializer) ||
                dynamic_cast<AssetRefStatistician*>(&serializer)
            )
            {
                AssetRef assetRef = AssetBundle::pointerToAssetSlot[reinterpret_cast<uintptr_t>(&value)]; //读取来自首次反序列化时保存的值或默认空值
                {
                    //优先获取目标对象的资源地址（序列化时保存），否则使用指针映射表存储的资源地址
                    assetRef = AssetBundle::GetAssetRef(value).value_or(assetRef);
                    assert(value == nullptr || !assetRef.assetBundleID.is_nil() && "指针引用的物体未被持久化！");
                    serializer.Transfer(assetRef); //序列化时写入或首次反序列化时从文件读取（PointerSerializer不执行传输）
                    value = static_cast<TValue*>(AssetBundle::GetObject(assetRef).value_or(nullptr)); //根据资源依赖获取数据
                }
                AssetBundle::pointerToAssetSlot[reinterpret_cast<uintptr_t>(&value)] = assetRef; //首次反序列化结束时保存来自资源文件的值
            }
            //统计未托管可持久化对象
            else if (ObjectRefStatistician* statistician = dynamic_cast<ObjectRefStatistician*>(&serializer))
            {
                if (value == nullptr || AssetBundle::objectToAssetSlot.contains(value))
                    return;

                auto optionalType = Type::GetType(typeid(*value));
                if (optionalType.has_value())
                {
                    statistician->dependencies.emplace_back(Asset{value, optionalType.value(), false});
                    statistician->dependencyUsers[value].emplace_back(reinterpret_cast<void**>(&value));
                }
            }
            //默认传输方式
            else
            {
                serializer.FallbackTransferPtr(*reinterpret_cast<void**>(&value), typeid(*value));
            }
        }
    };
}