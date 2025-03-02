#pragma once
#include "AssetBundle.h"
#include "AssetRef.h"
#include "GleamReflection/Runtime/Transferrer/DataTransferrer.h"

namespace Gleam
{
    //将指针类型同步到资源引用
    template <typename TValue>
    struct DataTransferrer_Transfer<TValue*>
    {
        static void Invoke(DataTransferrer& serializer, TValue*& value)
        {
            AssetRef assetRef = AssetBundle::pointerMapping[&value];
            {
                assetRef = AssetBundle::GetAssetRef(value).value_or(assetRef); //根据数据获取资源引用
                serializer.Transfer(assetRef);
                value = static_cast<TValue*>(AssetBundle::GetDataRef(assetRef).value_or(nullptr)); //根据资源引用获取数据
            }
            AssetBundle::pointerMapping[&value] = assetRef;
        }
    };

    /// 纯指针同步器
    /// 用于解决自引用资源包，因为加载顺序原因，导致指针未空的问题。
    /// 解决方法是每次资源包加载完毕后，用该同步器再过一遍。
    /// TODO 该功能和序列化器耦合了，好在序列化器默认不会序列化指针，但这显然是个隐患。
    class PointerTransferrer : public DataTransferrer
    {
    public:
        void PushNode(const char* name, DataType dataType) override
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