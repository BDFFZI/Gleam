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
#include "GleamReflection/Runtime/Type.h"
#include "GleamReflection/Runtime/Serialization/BinaryReader.h"
#include "GleamReflection/Runtime/Serialization/BinaryWriter.h"
#include "GleamReflection/Runtime/Serialization/JsonReader.h"
#include "GleamReflection/Runtime/Serialization/JsonWriter.h"


TEST(Reflection, BinarySerializer)
{
    Type& type = Type::GetType(typeid(TestData));

    std::ofstream outStream("test.bin", std::ios::binary);
    BinaryWriter binaryWriter = {outStream};

    type.Serialize()(binaryWriter, &data);
    outStream.close();

    std::ifstream inStream("test.bin", std::ios::binary);
    BinaryReader binaryReader = {inStream};
    TestData newData = {};
    type.Serialize()(binaryReader, &newData);
    inStream.close();

    ASSERT_EQ(newData, data);
}

TEST(Reflection, JsonSerializer)
{
    Type& type = Type::GetType(typeid(TestData));

    rapidjson::Document document;
    document.Parse("{}");

    JsonWriter jsonWriter = {document};
    type.Serialize()(jsonWriter, &data);

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter writer(buffer);
    document.Accept(writer);
    std::cout << buffer.GetString() << std::endl;

    JsonReader jsonReader = {document};
    TestData newData = {};
    type.Serialize()(jsonReader, &newData);

    ASSERT_EQ(newData, data);
}


TEST(Reflection, Type)
{
    std::optional<std::reference_wrapper<Type>> optionalType = Type::GetType("C4BAB34E-B145-4297-8BA3-6DD1BD05110D");
    if(optionalType.has_value() == false)
        throw std::runtime_error("无法通过Uuid找到类型！");
    Type& type = optionalType.value();

    std::cout << type.TypeInfo()->name() << '\n';
    for (auto& fieldInfo : type.FieldInfos())
    {
        std::cout << fieldInfo.name << '\t';
        std::cout << fieldInfo.type.name() << '\t';
        std::cout << fieldInfo.offset << '\t';
        std::cout << fieldInfo.size << '\n';
    }
}