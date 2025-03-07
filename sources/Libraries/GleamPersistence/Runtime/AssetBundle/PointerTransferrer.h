#pragma once
#include "GleamReflection/Runtime/FieldDataTransferrer.h"
#include "AssetRef.h"
#include "AssetBundle.h"

namespace Gleam
{
    /**
     * 将指针类型同步为资源引用
     *
     * TODO 该功能采用覆写传输函数的方法实现，这会影响到其他数据传输器
     * @tparam TValue 
     */
    template <typename TValue>
    struct FieldDataTransferrer_Transfer<TValue*>
    {
        static void Invoke(FieldDataTransferrer& serializer, TValue*& value)
        {
            AssetRef assetRef = AssetBundle::pointerMapping[&value];
            {
                assetRef = AssetBundle::GetAssetRef(value).value_or(assetRef); //获取引用数据对应的资源依赖
                assert(value == nullptr || !assetRef.assetBundleID.is_nil() && "指针引用的物体未被资源化！");
                serializer.Transfer(assetRef);
                value = static_cast<TValue*>(AssetBundle::GetDataRef(assetRef).value_or(nullptr)); //根据资源依赖获取数据
            }
            AssetBundle::pointerMapping[&value] = assetRef;
        }
    };

    /**
     * 一个不会实际进行数据传递的字段数据传输器，用于仅触发传输事件。
     *
     * 对于自引用的资源包，因为加载顺序原因，会导致指针为空。
     * 解决方法是每次资源包的数据加载完毕后，再单独处理一遍指针。
     */
    class PointerTransferrer : public FieldDataTransferrer
    {
    public:
        void PushNode(std::optional<std::string_view> name, DataType dataType) override
        {
        }
        void PopNode() override
        {
        }
        void Transfer(double& value) override
        {
        }
        void Transfer(int64_t& value) override
        {
        }
        void Transfer(std::string& value) override
        {
        }
        void Transfer(std::vector<std::byte>& value) override
        {
        }
    };
}