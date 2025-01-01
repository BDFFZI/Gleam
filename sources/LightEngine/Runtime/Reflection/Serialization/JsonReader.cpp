#include "JsonReader.h"

#include "LightEngine/Runtime/Utility/String.h"

void Light::JsonReader::Transfer(void* value, const std::type_index type)
{
#define MakeTransfer(valueType,function)\
if (type == typeid(valueType))\
{\
valueType##& originalValue = *static_cast<valueType##*>(value);\
originalValue = static_cast<valueType>(nodePath.top()->function);\
return;\
}

    MakeTransfer(float, GetFloat())
    MakeTransfer(int, GetInt())
    MakeTransfer(bool, GetBool())
    MakeTransfer(char, GetInt())
    MakeTransfer(std::string, GetString())
    MakeTransfer(size_t, GetUint64())
    MakeTransfer(uint32_t, GetUint())
#undef MakeTransfer

    if (type == typeid(std::vector<std::byte>))
    {
        std::vector<std::byte>& originalValue = *static_cast<std::vector<std::byte>*>(value);
        originalValue = String::DecodingBase64(nodePath.top()->GetString());
    }
}
void Light::JsonReader::PushPath(const char* name)
{
    nodePath.push(&nodePath.top()->operator[](name));
}
void Light::JsonReader::PopPath()
{
    nodePath.pop();
}