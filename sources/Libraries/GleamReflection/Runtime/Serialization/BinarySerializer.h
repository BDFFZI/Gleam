#pragma once
#include "../Transferrer/DataTransferrer.h"

namespace Gleam
{
    class BinarySerializer : public DataTransferrer
    {
    public:
        void PushNode(const char* name, DataType dataType) override
        {
        }
        void PopNode() override
        {
        }

        virtual void Transfer(char* address, int64_t size) = 0;

        template <class TValue>
        void TransferPrimitive(TValue& value)
        {
            Transfer(reinterpret_cast<char*>(&value), sizeof(TValue));
        }
        template <class TBuffer> requires
            requires(TBuffer& buffer, size_t size) { buffer.resize(size); std::size(buffer);std::data(buffer); }
        void TransferBuffer(TBuffer& buffer)
        {
            int64_t count = std::size(buffer);
            Transfer(reinterpret_cast<char*>(&count), sizeof(int64_t));
            buffer.resize(count);
            Transfer(reinterpret_cast<char*>(std::data(buffer)), count);
        }

        void Transfer(double& value) override { TransferPrimitive(value); }
        void Transfer(int64_t& value) override { TransferPrimitive(value); }
        void Transfer(std::string& value) override { TransferBuffer(value); }
        void Transfer(std::vector<std::byte>& value) override { TransferBuffer(value); }

        void Transfer(float& value) override { TransferPrimitive(value); }
        void Transfer(int32_t& value) override { TransferPrimitive(value); }
        void Transfer(uint64_t& value) override { TransferPrimitive(value); }
        void Transfer(uint32_t& value) override { TransferPrimitive(value); }
        void Transfer(bool& value) override { TransferPrimitive(value); }
        void Transfer(char& value) override { TransferPrimitive(value); }
    };
}