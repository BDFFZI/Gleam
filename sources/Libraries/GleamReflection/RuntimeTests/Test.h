#pragma once
#include "GleamReflection/Runtime/Type.h"

using namespace Gleam;

struct CustomBasicField
{
    int x, y, z;
    bool operator==(const CustomBasicField& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }
};

template <>
struct FieldDataTransferrer_Transfer<CustomBasicField>
{
    static void Invoke(FieldDataTransferrer& serializer, CustomBasicField& value)
    {
        serializer.PushNode(std::nullopt, DataType::Array);
        serializer.Transfer(value.x);
        serializer.Transfer(value.y);
        serializer.Transfer(value.z);
        serializer.PopNode();
    }
};


struct CustomClassField
{
    std::string name;
    CustomBasicField data;

    bool operator==(const CustomClassField& other) const
    {
        return name == other.name && data == other.data;
    }
};

Gleam_MakeType(CustomClassField, "21C1B5B4-B03B-4526-B770-1089E8501889")
{
    Gleam_MakeType_AddField(name);
    Gleam_MakeType_AddField(data);
}

struct CustomParent
{
    bool boolValue;
    char charValue;
    int intValue;
    float floatValue;
    std::string stringValue;
    std::vector<std::byte> bytesValue;
    CustomBasicField customBasicField;
    CustomClassField customClassField;
};

Gleam_MakeType(CustomParent, "95BFC556-6B59-4730-A756-9FC09F26A1A6")
{
    Gleam_MakeType_AddField(boolValue);
    Gleam_MakeType_AddField(charValue);
    Gleam_MakeType_AddField(intValue);
    Gleam_MakeType_AddField(floatValue);
    Gleam_MakeType_AddField(stringValue);
    Gleam_MakeType_AddField(bytesValue);
    Gleam_MakeType_AddField(customBasicField);
    Gleam_MakeType_AddField(customClassField);
}

struct CustomObject : CustomParent
{
    std::vector<int> vectorValue;
    std::vector<bool> boolVectorValue;
    std::vector<std::string> stringVectorValue;
    std::vector<CustomBasicField> customBasicFieldVectorValue;
    std::vector<CustomClassField> customClassFieldVectorValue;

    friend bool operator==(const CustomObject& lhs, const CustomObject& rhs)
    {
        return lhs.boolValue == rhs.boolValue
            && lhs.charValue == rhs.charValue
            && lhs.intValue == rhs.intValue
            && abs(lhs.floatValue - rhs.floatValue) < std::numeric_limits<float>::epsilon()
            && lhs.stringValue == rhs.stringValue
            && lhs.bytesValue == rhs.bytesValue
            && lhs.customBasicField == rhs.customBasicField
            && lhs.vectorValue == rhs.vectorValue
            && lhs.boolVectorValue == rhs.boolVectorValue
            && lhs.stringVectorValue == rhs.stringVectorValue
            && lhs.customBasicFieldVectorValue == rhs.customBasicFieldVectorValue
            && lhs.customClassFieldVectorValue == rhs.customClassFieldVectorValue;
    }
};

Gleam_MakeType2(CustomObject, "", CustomParentType)
{
    Gleam_MakeType_AddField(vectorValue);
    Gleam_MakeType_AddField(boolVectorValue);
    Gleam_MakeType_AddField(stringVectorValue);
    Gleam_MakeType_AddField(customBasicFieldVectorValue);
    Gleam_MakeType_AddField(customClassFieldVectorValue);
}

inline CustomObject data = {
    //基础类型
    true, 'A', 1, 0.5f, "Hello World!",
    {static_cast<std::byte>(1), static_cast<std::byte>(2), static_cast<std::byte>(3), static_cast<std::byte>(4)},
    CustomBasicField{1, 2, 3}, {"next is int3", {1, 1, 1}},
    //向量类型
    {3, 2, 1}, {false, true}, {"Hello 1", "Hello 2", "Hello 3",},
    {CustomBasicField{1, 0, 0}, CustomBasicField{0, 2, 0}, CustomBasicField{0, 0, 3}},
    {{"next is int3", {2, 2, 2}}}
};