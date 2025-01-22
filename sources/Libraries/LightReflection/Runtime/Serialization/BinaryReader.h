#pragma once
#include <istream>
#include "BinarySerializer.h"

namespace Light
{
    class BinaryReader : public BinarySerializer
    {
    public:
        BinaryReader(std::basic_istream<char>& stream)
            : stream(&stream)
        {
        }

        void Transfer(char* address, const int64_t size) override
        {
            stream->read(address, size);
        }

    private:
        std::basic_istream<char>* stream;
    };
}