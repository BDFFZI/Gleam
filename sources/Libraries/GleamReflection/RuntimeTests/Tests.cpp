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
#include "GleamReflection/Runtime/AssetBundle/AssetBundle.h"
#include "GleamReflection/Runtime/Serialization/JsonUtility.h"

struct TestAsset
{
    std::string name;
    TestAsset* dependency;
};

Gleam_MakeType(TestAsset, "A409ACFC-E52F-475F-A34F-B4361C59EF06")
{
    Gleam_MakeType_AddField(name);
    Gleam_MakeType_AddField(dependency);
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

    std::vector<Asset> assets;
    assets.emplace_back(0, TestAssetType.GetID(), &testAsset);
    assets.emplace_back(1, TestAssetType.GetID(), &testAsset);

    jsonWriter.TransferField("assets", assets);
    std::cout << JsonUtility::DocumentToJson(document, true) << std::endl;

    std::vector<Asset> newAssets;
    newAssets.emplace_back(-1, TestAssetType.GetID(), nullptr);
    newAssets.emplace_back(-1, TestAssetType.GetID(), nullptr);

    jsonReader.TransferField("assets", newAssets);

    ASSERT_EQ(static_cast<TestAsset*>(assets[0].GetData())->name, static_cast<TestAsset*>(newAssets[0].GetData())->name);
    ASSERT_EQ(static_cast<TestAsset*>(assets[1].GetData())->name, static_cast<TestAsset*>(newAssets[1].GetData())->name);
}

void main()
{
    //创建、保存、卸载资源包
    {
        TestAsset testAsset[] = {
            {"Asset1"},
            {"Asset2"},
            {"Asset3"}
        };

        AssetBundle& assetBundle = AssetBundle::Create(uuids::uuid::from_string("c57022f0-53a7-4b6b-99d5-41a1e5c8f51e").value());
        assetBundle.AddAsset(&testAsset[0], Type::GetType(typeid(TestAsset)));
        assetBundle.AddAsset(&testAsset[1], Type::GetType(typeid(TestAsset)));

        AssetBundle& assetBundle2 = AssetBundle::Create(uuids::uuid::from_string("c492a4ff-b846-4596-8a8d-09e25cba9b08").value());
        assetBundle2.AddAsset(&testAsset[2], Type::GetType(typeid(TestAsset)));

        testAsset[1].dependency = &testAsset[2];

        AssetBundle::SaveJson("Assets/assetBundle.asset", assetBundle);
        AssetBundle::SaveJson("Assets/assetBundle2.asset", assetBundle2);

        uuids::uuid uuid = AssetBundle::GetIDFromJson("Assets/assetBundle2.asset");
        ASSERT_EQ(uuid, assetBundle2.GetID());

        AssetBundle::Destroy(assetBundle);
        AssetBundle::Destroy(assetBundle2);
    }

    //加载资源包
    {
        AssetBundle& assetBundle = AssetBundle::Load(uuids::uuid::from_string("c57022f0-53a7-4b6b-99d5-41a1e5c8f51e").value());
        for (const Asset& asset : assetBundle.GetAssets())
        {
            std::cout << Type::GetType(asset.GetTypeID()).value().get().GetTypeInfo()->name() << std::endl;
            TestAsset& testAsset = *static_cast<TestAsset*>(asset.GetData());
            std::cout << testAsset.name << std::endl;
            std::cout << testAsset.dependency << std::endl;
            if (testAsset.dependency != nullptr)
                std::cout << testAsset.dependency->name << std::endl;
        }
    }
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