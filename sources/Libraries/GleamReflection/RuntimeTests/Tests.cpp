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


#include <unordered_set>
#include <vector>

#include "GleamReflection/Runtime/AssetBundle/Asset.h"
#include "GleamReflection/Runtime/Serialization/JsonUtility.h"

namespace Gleam
{
    

    // void DataTransferrer_Transfer<Asset>::Invoke(DataTransferrer& serializer, Asset& value)
    // {
    // }
    //
    // class AssetBundle
    // {
    // public:
    //     void AddAsset(void* asset, const Type& assetType);
    //     void RemoveAsset(void* asset);
    //
    // private:
    //     std::vector<Asset> assets;
    //     std::unordered_set<int> assetIDsSet;
    //     void Transfer(DataTransferrer& serializer)
    //     {
    //         for (Asset& asset : assets)
    //         {
    //             serializer.Transfer()
    //         }
    //     }
    // };
}

struct TestAsset
{
    std::string name;
};

Gleam_MakeType(TestAsset, "A409ACFC-E52F-475F-A34F-B4361C59EF06")
{
    Gleam_MakeType_AddField(name);
}

TEST(Reflection, Asset)
{
    rapidjson::Document document;
    document.Parse("{}");

    JsonWriter jsonWriter = {document};
    JsonReader jsonReader = {document};

    TestAsset testAsset[] = {
        {"Hi Asset1!"},
        {"Hello Asset2!"}
    };

    std::vector<Asset> assets = {
        {0, TestAssetType.GetUuid(), &testAsset},
        {1, TestAssetType.GetUuid(), &testAsset},
    };

    jsonWriter.TransferField("assets", assets);
    std::cout << JsonUtility::DocumentToJson(document, true) << std::endl;

    std::vector<Asset> newAssets = {
        {-1, TestAssetType.GetUuid(), nullptr},
        {-1, TestAssetType.GetUuid(), nullptr},
    };
    jsonReader.TransferField("assets", newAssets);

    ASSERT_EQ(static_cast<TestAsset*>(assets[0].data)->name, static_cast<TestAsset*>(newAssets[0].data)->name);
    ASSERT_EQ(static_cast<TestAsset*>(assets[1].data)->name, static_cast<TestAsset*>(newAssets[1].data)->name);
}

TEST(Reflection, BinarySerializer)
{
    Type& type = Type::GetType(typeid(TestData));

    std::ofstream outStream("test.bin", std::ios::binary);
    BinaryWriter binaryWriter = {outStream};

    type.GetSerialize()(binaryWriter, &data);
    outStream.close();

    std::ifstream inStream("test.bin", std::ios::binary);
    BinaryReader binaryReader = {inStream};
    TestData newData = {};
    type.GetSerialize()(binaryReader, &newData);
    inStream.close();

    ASSERT_EQ(newData, data);
}

TEST(Reflection, JsonSerializer)
{
    Type& type = Type::GetType(typeid(TestData));

    rapidjson::Document document;
    document.Parse("{}");

    JsonWriter jsonWriter = {document};
    type.GetSerialize()(jsonWriter, &data);

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter writer(buffer);
    document.Accept(writer);
    std::cout << buffer.GetString() << std::endl;

    JsonReader jsonReader = {document};
    TestData newData = {};
    type.GetSerialize()(jsonReader, &newData);

    ASSERT_EQ(newData, data);
}


TEST(Reflection, Type)
{
    std::optional<std::reference_wrapper<Type>> optionalType = Type::GetType("C4BAB34E-B145-4297-8BA3-6DD1BD05110D");
    if (optionalType.has_value() == false)
        throw std::runtime_error("无法通过Uuid找到类型！");
    Type& type = optionalType.value();

    std::cout << type.GetTypeInfo()->name() << '\n';
    for (auto& fieldInfo : type.GetFieldInfos())
    {
        std::cout << fieldInfo.name << '\t';
        std::cout << fieldInfo.type.name() << '\t';
        std::cout << fieldInfo.offset << '\t';
        std::cout << fieldInfo.size << '\n';
    }
}