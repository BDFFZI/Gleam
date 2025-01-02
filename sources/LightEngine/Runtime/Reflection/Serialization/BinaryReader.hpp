#pragma once
#include <istream>

#include "BinarySerializer.hpp"
#include "Serializer.hpp"

namespace Light
{
    class BinaryReader : public BinarySerializer
    {
    public:
        BinaryReader(std::basic_istream<char>& stream)
            : stream(&stream)
        {
        }

        void Transfer(void* value, const std::type_index type) override
        {
            Light_Transfer_Inner(Transfer)
            throw std::runtime_error("不支持的传输类型！");
        }
        template <class TValue>
        void Transfer(TValue& value)
        {
            stream->read(reinterpret_cast<char*>(&value), sizeof(TValue));
        }
        template <class TBuffer>
            requires requires(TBuffer& buffer) { buffer.resize(0);std::data(buffer); }
        void Transfer(TBuffer& buffer)
        {
            int count;
            Transfer(count);
            buffer.resize(count);
            stream->read(reinterpret_cast<char*>(std::data(buffer)), count);
        }

    private:
        std::basic_istream<char>* stream;
    };
}