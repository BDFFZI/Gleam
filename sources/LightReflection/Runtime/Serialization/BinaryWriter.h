#pragma once
#include <strstream>

#include "Transferrer.hpp"

namespace Light
{
    class BinaryWriter : public Transferrer
    {
    public:
        BinaryWriter(std::basic_ostream<char>& stream);

        void TransferValue(void* value, std::type_index type) override;

        template <class TValue>
        void Write(const TValue& value)
        {
            stream->write(reinterpret_cast<const char*>(&value), sizeof(TValue));
        }
        template <class TBuffer>
            requires requires(const TBuffer& buffer) { std::size(buffer);std::data(buffer); }
        void Write(const TBuffer& buffer)
        {
            int count = static_cast<int>(std::size(buffer));
            Write(count);
            stream->write(reinterpret_cast<const char*>(std::data(buffer)), count);
        }

    private:
        std::basic_ostream<char>* stream;
    };
}
