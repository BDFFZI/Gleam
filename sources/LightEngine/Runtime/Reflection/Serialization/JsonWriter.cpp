#include "JsonWriter.h"

#include "LightEngine/Runtime/Utility/String.h"

namespace Light
{
    void JsonWriter::Transfer(void* value, const std::type_index type)
    {
#define MakeTransfer(valueType,function)\
        if (type == typeid(valueType))\
        {\
            valueType##& originalValue = *static_cast<valueType##*>(value);\
            nodePath.top()->function;\
            return;\
        }
        MakeTransfer(float, SetFloat(originalValue))
        MakeTransfer(int, SetInt(originalValue))
        MakeTransfer(bool, SetBool(originalValue))
        MakeTransfer(char, SetInt(originalValue))
        MakeTransfer(std::string, SetString(originalValue.data(), originalValue.size(), allocator))
        MakeTransfer(size_t, SetUint64(originalValue))
        MakeTransfer(uint32_t, SetUint(originalValue))
#undef MakeTransfer
        
        if (type == typeid(std::vector<std::byte>))
        {
            std::vector<std::byte>& originalValue = *static_cast<std::vector<std::byte>*>(value);
            std::string stringValue = String::EncodingBase64(originalValue);
            nodePath.top()->SetString(stringValue.data(), static_cast<uint32_t>(stringValue.size()), allocator);
        }
    }
    void JsonWriter::PushPath(const char* name)
    {
        //获取当前节点
        rapidjson::Value& currentNode = *nodePath.top();
        if (currentNode.IsObject() == false) //嵌套推送路径，说明该节点是类对象
            currentNode.SetObject();
        //创建字段名
        rapidjson::GenericValue<rapidjson::UTF8<>> keyString(
            name, static_cast<rapidjson::SizeType>(strlen(name)), allocator);
        //添加字段
        currentNode.AddMember(keyString, 0, allocator);
        
        nodePath.push(&currentNode[name]);
    }
    void JsonWriter::PopPath()
    {
        nodePath.pop();
    }
}