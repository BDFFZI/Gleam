#include <deque>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <typeindex>
#include <gtest/gtest.h>
#include <ranges>

#include "LightMath/Runtime/VectorMath.hpp"
#include "LightMath/Runtime/Vector.hpp"
#include "LightReflection/Runtime/Type.hpp"
#include "LightReflection/Runtime/Serialization/BinaryReader.h"
#include "LightReflection/Runtime/Serialization/BinaryWriter.h"

using namespace Light;

struct Data
{
    bool boolValue;
    char charValue;
    int intValue;
    float floatValue;
    std::string stringValue;
    std::vector<bool> vectorValue;
    float3 customValue;

    friend bool operator==(const Data& lhs, const Data& rhs)
    {
        return lhs.boolValue == rhs.boolValue
            && lhs.charValue == rhs.charValue
            && lhs.intValue == rhs.intValue
            && abs(lhs.floatValue - rhs.floatValue) < std::numeric_limits<float>::epsilon()
            && lhs.stringValue == rhs.stringValue
            && lhs.vectorValue == rhs.vectorValue
            && all(lhs.customValue == rhs.customValue);
    }
};

template <class Type, int Number>
struct SerializerTransfer<vector<Type, Number>>
{
    static void Invoke(Serializer& transferrer, vector<Type, Number>& value)
    {
        for (int i = 0; i < Number; i++)
            transferrer.Transfer(value.data[i]);
    }
};

TEST(Reflection, BinaryTransferrer)
{
    std::ofstream outStream("test.bin", std::ios::binary);
    BinaryWriter binaryWriter = {outStream};
    Data oldData = {true, 'A', 1, 0.5f, "Hello World!", {false, true}, {1, 2, 3}};
    binaryWriter.TransferField("boolValue", oldData.boolValue);
    binaryWriter.TransferField("charValue", oldData.charValue);
    binaryWriter.TransferField("intValue", oldData.intValue);
    binaryWriter.TransferField("floatValue", oldData.floatValue);
    binaryWriter.TransferField("stringValue", oldData.stringValue);
    binaryWriter.TransferField("vectorValue", oldData.vectorValue);
    binaryWriter.TransferField("customValue", oldData.customValue);
    outStream.close();

    std::ifstream inStream("test.bin", std::ios::binary);
    BinaryReader binaryReader = {inStream};
    Data newData = {};
    binaryReader.TransferField("boolValue", newData.boolValue);
    binaryReader.TransferField("charValue", newData.charValue);
    binaryReader.TransferField("intValue", newData.intValue);
    binaryReader.TransferField("floatValue", newData.floatValue);
    binaryReader.TransferField("stringValue", newData.stringValue);
    binaryReader.TransferField("vectorValue", newData.vectorValue);
    binaryReader.TransferField("customValue", newData.customValue);
    inStream.close();

    ASSERT_EQ(newData, oldData);
}

MakeType("C4BAB34E-B145-4297-8BA3-6DD1BD05110D", Data)
{
    MakeType_AddField(boolValue);
    MakeType_AddField(charValue);
    MakeType_AddField(intValue);
    MakeType_AddField(floatValue);
    MakeType_AddField(stringValue);
    MakeType_AddField(vectorValue);
    MakeType_AddField(customValue);
}

TEST(Reflection, Type)
{
    uuids::uuid uuid = uuids::uuid::from_string("C4BAB34E-B145-4297-8BA3-6DD1BD05110D").value();
    Type* type = Type::uuidToType[uuid];

    std::cout << type->info->name() << '\n';
    for (auto& fieldInfo : type->fieldInfos)
    {
        std::cout << fieldInfo.name << '\t';
        std::cout << fieldInfo.type.name() << '\t';
        std::cout << fieldInfo.offset << '\t';
        std::cout << fieldInfo.size << '\n';
    }
}
