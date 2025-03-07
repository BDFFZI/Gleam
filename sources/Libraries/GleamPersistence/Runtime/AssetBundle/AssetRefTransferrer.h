#pragma once
#include "AssetRef.h"
#include "GleamReflection/Runtime/FieldDataTransferrer.h"

namespace Gleam
{
    /**
     * 用于统计资源包中用到的AssetRef
     */
    class AssetRefStatistician : public FieldDataTransferrer
    {
    public:
        std::vector<AssetRef> result;

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
        void Transfer(void* value, const std::type_index typeIndex) override
        {
            if (typeIndex == typeid(AssetRef))
            {
                AssetRef& assetRef = *static_cast<AssetRef*>(value);
                if (!assetRef.assetBundleID.is_nil())
                    result.push_back(assetRef);
            }
            else
                FieldDataTransferrer::Transfer(value, typeIndex);
        }
    };
}