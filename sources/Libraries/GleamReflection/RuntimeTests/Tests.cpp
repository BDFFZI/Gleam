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

struct CustomComponent1
{
    int intValue = 3;
};
Gleam_MakeType(CustomComponent1, "F3CEFD0B-0FDE-40B3-A8D4-B233419095B0")
{
    Gleam_MakeType_AddField(intValue);
}
struct CustomComponent2
{
    std::string stringValue = "Hello world!";
};
Gleam_MakeType(CustomComponent2, "6DEF6625-C2B2-49D7-A045-251C30DEBD1D")
{
    Gleam_MakeType_AddField(stringValue);
}
Gleam_MakeCombinedType(CustomArchetype, CustomComponent1Type, CustomComponent2Type)

TEST(Reflection, CombinedType)
{
    std::cout << CustomArchetype.GetID() << '\n';
    for (auto& fieldInfo : CustomArchetype.GetFields())
    {
        std::cout << fieldInfo.name << '\t';
        std::cout << fieldInfo.type.name() << '\t';
        std::cout << fieldInfo.offset << '\t';
        std::cout << fieldInfo.size << '\n';
    }
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

    ASSERT_EQ(static_cast<TestAsset*>(assets[0].GetDataRef())->name, static_cast<TestAsset*>(newAssets[0].GetDataRef())->name);
    ASSERT_EQ(static_cast<TestAsset*>(assets[1].GetDataRef())->name, static_cast<TestAsset*>(newAssets[1].GetDataRef())->name);
}

TEST(Reflection, AssetBundle)
{
    uuids::uuid assetBundleID = uuids::uuid::from_string("c57022f0-53a7-4b6b-99d5-41a1e5c8f51e").value();
    uuids::uuid assetBundle2ID = uuids::uuid::from_string("c492a4ff-b846-4596-8a8d-09e25cba9b08").value();

    //创建、保存、卸载资源包
    {
        TestAsset testAsset[] = {
            {"Asset1"},
            {"Asset2"},
            {"Asset3"}
        };

        AssetBundle& assetBundle = AssetBundle::Create(assetBundleID);
        assetBundle.AddAsset(&testAsset[0], TestAssetType);
        assetBundle.AddAsset(&testAsset[1], TestAssetType);

        AssetBundle& assetBundle2 = AssetBundle::Create(assetBundle2ID);
        assetBundle2.AddAsset(&testAsset[2], TestAssetType);

        testAsset[1].dependency = &testAsset[2];

        AssetBundle::SaveJson("Assets/assetBundle.asset", assetBundle);
        AssetBundle::SaveJson("Assets/assetBundle2.asset", assetBundle2);

        uuids::uuid uuid = AssetBundle::GetIDFromJson("Assets/assetBundle2.asset");
        ASSERT_EQ(uuid, assetBundle2.GetID());

        AssetBundle::UnLoad(assetBundle);
        AssetBundle::UnLoad(assetBundle2);
    }

    //加载资源包
    {
        TestAsset testAsset[] = {
            {"Asset1"},
            {"Asset2"},
            {"Asset3"}
        };

        AssetBundle& assetBundle = AssetBundle::Load(assetBundleID);
        for (int i = 0; const Asset& asset : assetBundle.GetAssets())
        {
            Type& type = Type::GetType(asset.GetTypeID()).value().get();
            ASSERT_EQ(&type, &TestAssetType);
            TestAsset& data = *static_cast<TestAsset*>(asset.GetDataRef());
            ASSERT_EQ(data.name, testAsset[i].name);
            i++;
        }

        //获取第二个自动加载的资源包
        ASSERT_TRUE(AssetBundle::HasInMemory(assetBundle2ID)); //第二个资源包被依赖，自动加载
        TestAsset& data3 = *static_cast<TestAsset*>(assetBundle.GetAssets()[1].GetDataRef())->dependency;
        AssetRef assetRef = AssetBundle::GetAssetRef(&data3).value();
        AssetBundle& assetBundle2 = AssetBundle::GetAssetBundle(assetRef.assetBundleID);

        //测试内容重载
        ASSERT_EQ(data3.name, testAsset[2].name);
        data3.name += "Error";
        AssetBundle::Load(assetRef.assetBundleID, true);
        ASSERT_EQ(data3.name, testAsset[2].name);

        //移动资源后保存
        Asset asset = assetBundle.ExtractAsset(assetBundle.GetAssets()[1].GetID());
        Asset asset2 = assetBundle2.ExtractAsset(assetBundle.GetAssets()[0].GetID());
        assetBundle2.EmplaceAsset(std::move(asset));
        assetBundle2.EmplaceAsset(std::move(asset2));
        AssetBundle::SaveJson("Assets/assetBundle.asset", assetBundle);
        data3.name = "NewAsset3";
        AssetBundle::SaveJson("Assets/assetBundle2.asset", assetBundle2, false);
        AssetBundle::DumpJson("Assets/assetBundle2.asset");

        AssetBundle::UnLoad(assetBundle);
        AssetBundle::UnLoad(assetBundle2);
    }

    //加载自依赖资源包
    {
        AssetBundle& assetBundle2 = AssetBundle::Load(assetBundle2ID);
        ASSERT_TRUE(!AssetBundle::HasInMemory(assetBundleID)); //第一个资源包无依赖，不应加载
        ASSERT_EQ(assetBundle2.GetData<TestAsset>(0).dependency->name, "NewAsset3");
    }
}

TEST(Reflection, BinarySerializer)
{
    Type& type = Type::GetType(typeid(CustomData)).value();

    std::ofstream outStream("test.bin", std::ios::binary);
    BinaryWriter binaryWriter = {outStream};

    type.GetSerialize()(binaryWriter, &data);
    outStream.close();

    std::ifstream inStream("test.bin", std::ios::binary);
    BinaryReader binaryReader = {inStream};
    CustomData newData = {};
    type.GetSerialize()(binaryReader, &newData);
    inStream.close();

    ASSERT_EQ(newData, data);
}

TEST(Reflection, JsonSerializer)
{
    Type& type = Type::GetType(typeid(CustomData)).value();

    rapidjson::Document document;
    document.Parse("{}");

    JsonWriter jsonWriter = {document};
    type.GetSerialize()(jsonWriter, &data);

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter writer(buffer);
    document.Accept(writer);
    std::cout << buffer.GetString() << std::endl;

    JsonReader jsonReader = {document};
    CustomData newData = {};
    type.GetSerialize()(jsonReader, &newData);

    ASSERT_EQ(newData, data);
}


TEST(Reflection, Type)
{
    Type& type = Type::GetType(CustomDataType.GetID()).value().get();
    std::cout << type.GetID() << '\n';
    for (auto& fieldInfo : type.GetFields())
    {
        std::cout << fieldInfo.name << '\t';
        std::cout << fieldInfo.type.name() << '\t';
        std::cout << fieldInfo.offset << '\t';
        std::cout << fieldInfo.size << '\n';
    }
}