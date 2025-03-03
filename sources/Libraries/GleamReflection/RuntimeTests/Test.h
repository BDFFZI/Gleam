#pragma once
#include "GleamMath/Runtime/LinearAlgebra/Matrix.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"

using namespace Gleam;

struct CustomField
{
    std::string name;
    float4x4 data;

    bool operator==(const CustomField& other) const
    {
        return name == other.name && all(data == other.data);
    }
};

Gleam_MakeType(CustomField, "21C1B5B4-B03B-4526-B770-1089E8501889")
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
    float3 customValue;
};

Gleam_MakeType(CustomParent, "95BFC556-6B59-4730-A756-9FC09F26A1A6")
{
    Gleam_MakeType_AddField(boolValue);
    Gleam_MakeType_AddField(charValue);
    Gleam_MakeType_AddField(intValue);
    Gleam_MakeType_AddField(floatValue);
    Gleam_MakeType_AddField(stringValue);
    Gleam_MakeType_AddField(bytesValue);
    Gleam_MakeType_AddField(customValue);
}

struct CustomData : CustomParent
{
    std::vector<int> vectorValue;
    std::vector<bool> boolVectorValue;
    std::vector<std::string> stringVectorValue;
    std::vector<float3> customVectorValue;
    std::vector<CustomField> customClassVectorValue;

    friend bool operator==(const CustomData& lhs, const CustomData& rhs)
    {
        return lhs.boolValue == rhs.boolValue
            && lhs.charValue == rhs.charValue
            && lhs.intValue == rhs.intValue
            && abs(lhs.floatValue - rhs.floatValue) < std::numeric_limits<float>::epsilon()
            && lhs.stringValue == rhs.stringValue
            && lhs.bytesValue == rhs.bytesValue
            && all(lhs.customValue == rhs.customValue)
            && lhs.vectorValue == rhs.vectorValue
            && lhs.boolVectorValue == rhs.boolVectorValue
            && lhs.stringVectorValue == rhs.stringVectorValue
            && std::ranges::equal(
                lhs.customVectorValue,
                rhs.customVectorValue,
                [](const float3 l, const float3 r)
                {
                    return all(l == r);
                }
            )
            && lhs.customClassVectorValue == rhs.customClassVectorValue;
    }
};

Gleam_MakeType2(CustomData, "C4BAB34E-B145-4297-8BA3-6DD1BD05110D", CustomParentType)
{
    Gleam_MakeType_AddField(vectorValue);
    Gleam_MakeType_AddField(boolVectorValue);
    Gleam_MakeType_AddField(stringVectorValue);
    Gleam_MakeType_AddField(customVectorValue);
    Gleam_MakeType_AddField(customClassVectorValue);
}

inline CustomData data = {
    //基础类型
    true, 'A', 1, 0.5f, "Hello World!",
    {static_cast<std::byte>(1), static_cast<std::byte>(2), static_cast<std::byte>(3), static_cast<std::byte>(4)},
    float3{1, 2, 3},
    //向量类型
    {3, 2, 1}, {false, true}, {"Hello 1", "Hello 2", "Hello 3",},
    {float3{1, 0, 0}, float3{0, 2, 0}, float3{0, 0, 3}},
    {{"one matrix", 2}}
};