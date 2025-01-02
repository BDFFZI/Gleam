#include "JsonWriter.h"

#include "LightEngine/Runtime/Utility/String.h"

namespace Light
{
    void JsonWriter::Transfer(void* value, const std::type_index type)
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
    #undef MakeTransfer

        throw std::runtime_error("不支持的传输类型！");
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