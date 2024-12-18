#pragma once
#include <istream>

#include "Serializer.hpp"

namespace Light
{
    class BinaryReader : public Serializer
    {
    public:
        BinaryReader(std::basic_istream<char>& stream);

        void Transfer(void* value, std::type_index type) override;

        template <class TValue>
        void Read(TValue& value)
        {
            stream->read(reinterpret_cast<char*>(&value), sizeof(TValue));
        }
        template <class TBuffer>
            requires requires(TBuffer& buffer) { buffer.resize(0);std::data(buffer); }
        void Read(TBuffer& buffer)
        {
            int count;
            Read(count);
            buffer.resize(count);
            stream->read(reinterpret_cast<char*>(std::data(buffer)), count);
        }

    private:
        std::basic_istream<char>* stream;
    };
}
