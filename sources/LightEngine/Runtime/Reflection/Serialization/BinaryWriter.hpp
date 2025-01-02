#pragma once
#include <strstream>

#include "BinarySerializer.hpp"
#include "Serializer.hpp"

namespace Light
{
    class BinaryWriter : public BinarySerializer
    {
    public:
        BinaryWriter(std::basic_ostream<char>& stream)
            : stream(&stream)
        {
        }

        void Transfer(void* value, const std::type_index type) override
        {
            Light_Transfer_Inner(Transfer)
            throw std::runtime_error("不支持的传输类型！");
        }
        template <class TValue>
        void Transfer(const TValue& value)
        {
            stream->write(reinterpret_cast<const char*>(&value), sizeof(TValue));
        }
        template <class TBuffer>
            requires requires(const TBuffer& buffer) { std::size(buffer);std::data(buffer); }
        void Transfer(const TBuffer& buffer)
        {
            int count = static_cast<int>(std::size(buffer));
            Transfer(count);
            stream->write(reinterpret_cast<const char*>(std::data(buffer)), count);
        }

    private:
        std::basic_ostream<char>* stream;
    };
}