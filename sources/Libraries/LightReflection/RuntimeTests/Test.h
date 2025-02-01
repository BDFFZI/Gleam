#pragma once
#include "LightMath/Runtime/LinearAlgebra/Matrix.h"
#include "LightMath/Runtime/LinearAlgebra/MatrixMath.h"

using namespace Light;

struct TestClass
{
    std::string name;
    float4x4 data;

    bool operator==(const TestClass& other) const
    {
        return name == other.name && all(data == other.data);
    }
};

Light_MakeType(TestClass, "")
{
    Light_MakeType_AddField(name);
    Light_MakeType_AddField(data);
}


struct TestData
{
    bool boolValue;
    char charValue;
    int intValue;
    float floatValue;
    std::string stringValue;
    std::vector<std::byte> bytesValue;
    float3 customValue;
    std::vector<int> vectorValue;
    std::vector<bool> boolVectorValue;
    std::vector<std::string> stringVectorValue;
    std::vector<float3> customVectorValue;
    std::vector<TestClass> customClassVectorValue;

    friend bool operator==(const TestData& lhs, const TestData& rhs)
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


Light_MakeType(TestData, "C4BAB34E-B145-4297-8BA3-6DD1BD05110D")
{
    Light_MakeType_AddField(boolValue);
    Light_MakeType_AddField(charValue);
    Light_MakeType_AddField(intValue);
    Light_MakeType_AddField(floatValue);
    Light_MakeType_AddField(stringValue);
    Light_MakeType_AddField(bytesValue);
    Light_MakeType_AddField(customValue);
    Light_MakeType_AddField(vectorValue);
    Light_MakeType_AddField(boolVectorValue);
    Light_MakeType_AddField(stringVectorValue);
    Light_MakeType_AddField(customVectorValue);
    Light_MakeType_AddField(customClassVectorValue);
}

inline TestData data = {
    //基础类型
    true, 'A', 1, 0.5f, "Hello World!",
    {static_cast<std::byte>(1), static_cast<std::byte>(2), static_cast<std::byte>(3), static_cast<std::byte>(4)},
    float3{1, 2, 3},
    //向量类型
    {3, 2, 1}, {false, true}, {"Hello 1", "Hello 2", "Hello 3",},
    {float3{1, 0, 0}, float3{0, 2, 0}, float3{0, 0, 3}},
    {{"one matrix", 2}}
};