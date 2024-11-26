#include <deque>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <typeindex>
#include <gtest/gtest.h>

#include "LightMath/Runtime/VectorMath.hpp"
#include "LightMath/Runtime/Vector.hpp"
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
struct ValueTransferrer<vector<Type, Number>>
{
    static void TransferValue(Transferrer& transferrer, vector<Type, Number>& value)
    {
        for (int i = 0; i < Number; i++)
            transferrer.TransferValue(value.data[i]);
    }
};

TEST(Reflection, Data)
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
