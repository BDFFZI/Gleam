#include "BinaryReader.h"

namespace Light
{
    BinaryReader::BinaryReader(std::basic_istream<char>& stream)
        : stream(&stream)
    {
    }
    void BinaryReader::TransferValue(void* value, const std::type_index type)
    {
#define MakeTransfer(valueType) if (type == typeid(valueType)){Read(*static_cast<valueType##*>(value));return;}
        MakeTransfer(float)
        MakeTransfer(int)
        MakeTransfer(bool)
        MakeTransfer(char)
        MakeTransfer(std::string)
        MakeTransfer(std::vector<std::byte>)
        MakeTransfer(size_t)
        MakeTransfer(uint32_t)
#undef MakeTransfer

        throw std::runtime_error("不支持的传输类型！");
    }
}
