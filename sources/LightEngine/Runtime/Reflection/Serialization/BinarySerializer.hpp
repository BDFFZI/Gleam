#pragma once
#include "../Transferrer/DataTransferrer.h"

namespace Light
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
        void TransferT(TValue& value)
        {
            Transfer(reinterpret_cast<char*>(&value), sizeof(TValue));
        }
        template <class TBuffer> requires
            requires(TBuffer& buffer) { buffer.resize(); std::size(buffer);std::data(buffer); }
        void TransferT(TBuffer& buffer)
        {
            int64_t count = std::size(buffer);
            Transfer(reinterpret_cast<char*>(&count), sizeof(int64_t));
            buffer.resize();
            Transfer(reinterpret_cast<char*>(std::data(buffer)), count);
        }

        void Transfer(double& value) override { TransferT(value); }
        void Transfer(int64_t& value) override { TransferT(value); }
        void Transfer(std::string& value) override { TransferT(value); }
        void Transfer(std::vector<std::byte>& value) override { TransferT(value); }

        void Transfer(float& value) override { TransferT(value); }
        void Transfer(int32_t& value) override { TransferT(value); }
        void Transfer(uint64_t& value) override { TransferT(value); }
        void Transfer(uint32_t& value) override { TransferT(value); }
        void Transfer(bool& value) override { TransferT(value); }
        void Transfer(char& value) override { TransferT(value); }
    };
}