#include <iostream>
#include <thread>

#include <fstream>
#include <ostream>
#include <string>
#include <typeindex>
#include <gtest/gtest.h>
#include <ranges>
#include <rapidjson/prettywriter.h>
#include "LightReflection/Runtime/Type.h"
#include "LightReflection/Runtime/Serialization/BinaryReader.h"
#include "LightReflection/Runtime/Serialization/BinaryWriter.h"
#include "LightReflection/Runtime/Serialization/JsonReader.h"
#include "LightReflection/Runtime/Serialization/JsonWriter.h"
#include "LightMath/Runtime/Vector.h"
#include "LightMath/Runtime/VectorMath.h"

using namespace Light;

struct Data
{
    bool boolValue;
    char charValue;
    int intValue;
    float floatValue;
    std::vector<int> vectorValue;
    std::vector<bool> boolVectorValue;
    std::string stringValue;
    std::vector<std::string> stringVectorValue;
    std::vector<std::byte> bytesValue;
    float3 customValue;
    std::vector<float3> customVectorValue;

    friend bool operator==(const Data& lhs, const Data& rhs)
    {
        return lhs.boolValue == rhs.boolValue
            && lhs.charValue == rhs.charValue
            && lhs.intValue == rhs.intValue
            && abs(lhs.floatValue - rhs.floatValue) < std::numeric_limits<float>::epsilon()
            && lhs.vectorValue == rhs.vectorValue
            && lhs.boolVectorValue == rhs.boolVectorValue
            && lhs.stringValue == rhs.stringValue
            && lhs.stringVectorValue == rhs.stringVectorValue
            && lhs.bytesValue == rhs.bytesValue
            && all(lhs.customValue == rhs.customValue)
            && std::ranges::equal(
                lhs.customVectorValue,
                rhs.customVectorValue,
                [](const float3 l, const float3 r)
                {
                    return all(l == r);
                }
            );
    }
    void Transfer(DataTransferrer& serializer)
    {
        serializer.TransferField("boolValue", boolValue);
        serializer.TransferField("charValue", charValue);
        serializer.TransferField("intValue", intValue);
        serializer.TransferField("floatValue", floatValue);
        serializer.TransferField("vectorValue", vectorValue);
        serializer.TransferField("stringValue", stringValue);
        serializer.TransferField("stringVectorValue", stringVectorValue);
        serializer.TransferField("boolVectorValue", boolVectorValue);
        serializer.TransferField("bytesValue", bytesValue);
        serializer.TransferField("customValue", customValue);
        serializer.TransferField("customVectorValue", customVectorValue);
    }
};

Data oldData = {true,
    'A', 1, 0.5f, {3, 2, 1},
    {false, true},
    "Hello World!",
    {"Hello 1", "Hello 2", "Hello 3",},
    {static_cast<std::byte>(1), static_cast<std::byte>(2), static_cast<std::byte>(3), static_cast<std::byte>(4)},
    float3{1, 2, 3},
    {float3{1, 0, 0}, float3{0, 2, 0}, float3{0, 0, 3}}
};

TEST(Reflection, BinarySerializer)
{
    std::ofstream outStream("test.bin", std::ios::binary);
    BinaryWriter binaryWriter = {outStream};

    oldData.Transfer(binaryWriter);
    outStream.close();

    std::ifstream inStream("test.bin", std::ios::binary);
    BinaryReader binaryReader = {inStream};
    Data newData = {};
    newData.Transfer(binaryReader);
    inStream.close();

    ASSERT_EQ(newData, oldData);
}

TEST(Reflection, JsonSerializer)
{
    rapidjson::Document document;
    document.Parse("{}");

    JsonWriter jsonWriter = {document};
    oldData.Transfer(jsonWriter);

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter writer(buffer);
    document.Accept(writer);
    std::cout << buffer.GetString() << std::endl;

    JsonReader jsonReader = {document};
    Data newData = {};
    newData.Transfer(jsonReader);

    ASSERT_EQ(newData, oldData);
}

Light_MakeType(Data, "C4BAB34E-B145-4297-8BA3-6DD1BD05110D")
{
    Light_MakeType_AddField(boolValue);
    Light_MakeType_AddField(charValue);
    Light_MakeType_AddField(intValue);
    Light_MakeType_AddField(floatValue);
    Light_MakeType_AddField(stringValue);
    Light_MakeType_AddField(vectorValue);
    Light_MakeType_AddField(customValue);
}

TEST(Reflection, Type)
{
    Type* type = Type::GetType("C4BAB34E-B145-4297-8BA3-6DD1BD05110D");
    std::cout << type->info->name() << '\n';
    for (auto& fieldInfo : type->fieldInfos)
    {
        std::cout << fieldInfo.name << '\t';
        std::cout << fieldInfo.type.name() << '\t';
        std::cout << fieldInfo.offset << '\t';
        std::cout << fieldInfo.size << '\n';
    }
}