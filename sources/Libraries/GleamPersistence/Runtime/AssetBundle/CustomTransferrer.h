#pragma once
#include "GleamReflection/Runtime/FieldDataTransferrer.h"
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    /**
     * 统计资源包中AssetRef的使用情况，可以此计算资源包依赖
     */
    class AssetRefStatistician : public FieldDataTransferrer
    {
    public:
        std::vector<AssetRef> result;

        void Transfer(void* value, const std::type_index typeIndex) override
        {
            if (typeIndex == typeid(AssetRef))
            {
                AssetRef& assetRef = *static_cast<AssetRef*>(value);
                if (!assetRef.assetBundleID.is_nil())
                    result.push_back(assetRef);
                return;
            }

            FieldDataTransferrer::Transfer(value, typeIndex);
        }
    };

    /**
     * 统计资源包中对象的指针引用的未托管对象，可以此计算要额外自动添加到资源包中的对象。
     *
     * 具体的统计函数由FieldDataTransferrer_Transfer实现
     */
    class PointerStatistician : public FieldDataTransferrer
    {
    public:
        std::vector<std::tuple<void*, std::type_index>> dependencies; //指针引用的未托管对象
        std::unordered_map<void*, std::vector<void**>> dependencyUsers; //引用这些对象的指针
    };

    /**
     * 一个不会进行实际传输的序列化器，仅用于触发指针序列化事件，以解决因序列化顺序导致指针无法引用到对象的问题
     */
    class PointerSerializer : public FieldDataTransferrer
    {
    };

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
                AssetRef assetRef = AssetBundle::pointerMapping[reinterpret_cast<uintptr_t>(&value)];
                {
                    assetRef = AssetBundle::GetAssetRef(value).value_or(assetRef); //获取引用数据对应的资源依赖
                    assert(value == nullptr || !assetRef.assetBundleID.is_nil() && "指针引用的物体未被资源化！");
                    serializer.Transfer(assetRef);
                    value = static_cast<TValue*>(AssetBundle::GetObject(assetRef).value_or(nullptr)); //根据资源依赖获取数据
                }
                AssetBundle::pointerMapping[reinterpret_cast<uintptr_t>(&value)] = assetRef;
            }
            //统计指针
            else if (PointerStatistician* statistician = dynamic_cast<PointerStatistician*>(&serializer))
            {
                if (value == nullptr)
                    return;
                if (AssetBundle::dataToAsset.contains(value))
                    return;

                statistician->dependencies.emplace_back(value, typeid(TValue));
                statistician->dependencyUsers[value].emplace_back(reinterpret_cast<void**>(&value));
            }
            //默认传输方式
            else
            {
                serializer.Transfer(&value, typeid(value));
            }
        }
    };
}