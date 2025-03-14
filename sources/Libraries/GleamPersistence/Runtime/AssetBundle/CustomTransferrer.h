#pragma once
#include "GleamReflection/Runtime/FieldDataTransferrer.h"
#include "GleamReflection/Runtime/Type.h"
#include "AssetBundle.h"

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
                serializer.Transfer(&value, typeid(value));
            }
        }
    };
}