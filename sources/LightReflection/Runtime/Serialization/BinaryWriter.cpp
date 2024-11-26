#include "BinaryWriter.h"

namespace Light
{
    BinaryWriter::BinaryWriter(std::basic_ostream<char>& stream): stream(&stream)
    {
    }

    void BinaryWriter::TransferValue(void* value, const std::type_index type)
    {
#define MakeTransfer(valueType) if (type == typeid(valueType)){Write(*static_cast<valueType##*>(value));return;}
        MakeTransfer(float)
        MakeTransfer(int)
        MakeTransfer(bool)
        MakeTransfer(char)
        MakeTransfer(std::string)
        MakeTransfer(std::vector<std::byte>)
        MakeTransfer(size_t)
        MakeTransfer(uint32_t)
        MakeTransfer(uint32_t)
    #undef MakeTransfer

        throw std::runtime_error("不支持的传输类型！");
    }
}