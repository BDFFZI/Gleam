#pragma once
#include <ostream>
#include "BinarySerializer.hpp"

namespace Light
{
    class BinaryWriter : public BinarySerializer
    {
    public:
        BinaryWriter(std::basic_ostream<char>& stream)
            : stream(&stream)
        {
        }

        void Transfer(char* address, const int64_t size) override
        {
            stream->write(address, size);
        }

    private:
        std::basic_ostream<char>* stream;
    };
}