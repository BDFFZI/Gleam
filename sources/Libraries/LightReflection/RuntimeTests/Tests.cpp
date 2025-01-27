#include <iostream>
#include <thread>

#include <fstream>
#include <ostream>
#include <string>
#include <typeindex>
#include <gtest/gtest.h>
#include <ranges>
#include <rapidjson/prettywriter.h>

#include "Test.h"
#include "LightReflection/Runtime/Type.h"
#include "LightReflection/Runtime/Serialization/BinaryReader.h"
#include "LightReflection/Runtime/Serialization/BinaryWriter.h"
#include "LightReflection/Runtime/Serialization/JsonReader.h"
#include "LightReflection/Runtime/Serialization/JsonWriter.h"


TEST(Reflection, BinarySerializer)
{
    Type* type = Type::GetType(typeid(TestData));

    std::ofstream outStream("test.bin", std::ios::binary);
    BinaryWriter binaryWriter = {outStream};

    type->serialize(binaryWriter, &data);
    outStream.close();

    std::ifstream inStream("test.bin", std::ios::binary);
    BinaryReader binaryReader = {inStream};
    TestData newData = {};
    type->deserialize(binaryReader, &newData);
    inStream.close();

    ASSERT_EQ(newData, data);
}

TEST(Reflection, JsonSerializer)
{
    Type* type = Type::GetType(typeid(TestData));

    rapidjson::Document document;
    document.Parse("{}");

    JsonWriter jsonWriter = {document};
    type->serialize(jsonWriter, &data);

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter writer(buffer);
    document.Accept(writer);
    std::cout << buffer.GetString() << std::endl;

    JsonReader jsonReader = {document};
    TestData newData = {};
    type->serialize(jsonReader, &newData);

    ASSERT_EQ(newData, data);
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