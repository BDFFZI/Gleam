#pragma once
#include "GleamReflection/Runtime/FieldDataTransferrer.h"

namespace Gleam
{
    /**
     * 一种接口，强迫子类必须完整实现传输器，同时便于后续可以利用动态类型转换确认传输器的类型
     */
    class Serializer : public FieldDataTransferrer
    {
    public:
        void PushNode(std::optional<std::string_view> name, DataType dataType) override =0;
        void PopNode() override =0;
        void Transfer(double& value) override = 0;
        void Transfer(int64_t& value) override = 0;
        void Transfer(std::string& value) override = 0;
        void Transfer(std::vector<std::byte>& value) override = 0;
        void FallbackTransferPtr(std::weak_ptr<void>& value, std::type_index objectType) override
        {
            throw std::runtime_error("不支持的传输类型！");
        }
        void FallbackTransfer(void* value, const std::type_index typeIndex) override
        {
            auto optionalType = Type::GetType(typeIndex);
            if (optionalType.has_value())
            {
                const Type& type = optionalType.value();
                PushNode(std::nullopt, DataType::Class);
                type.Serialize(*this, value);
                PopNode();
            }
            else
                throw std::runtime_error("不支持的传输类型！");
        }
    };
}