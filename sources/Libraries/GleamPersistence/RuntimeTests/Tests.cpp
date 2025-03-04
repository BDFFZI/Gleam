#include <fstream>
#include <gtest/gtest.h>
#include <rapidjson/stringbuffer.h>

#include "GleamPersistence/Runtime/AssetBundle/Asset.h"
#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"
#include "GleamPersistence/Runtime/Serializer/BinaryReader.h"
#include "GleamPersistence/Runtime/Serializer/BinaryWriter.h"
#include "GleamPersistence/Runtime/Serializer/JsonReader.h"
#include "GleamPersistence/Runtime/Serializer/JsonWriter.h"
#include "GleamReflection/Runtime/Type.h"
#include "GleamReflection/RuntimeTests/Test.h"

using namespace Gleam;

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
    Type& type = Type::CreateOrGet<TestAsset>();

    rapidjson::Document document;
    document.Parse("{}");
    JsonWriter jsonWriter = {document};
    JsonReader jsonReader = {document};

    //装载资源
    TestAsset testAsset[] = {
        {"Hi Asset1!"},
        {"Hello Asset2!"}
    };
    std::vector<Asset> assets;
    assets.emplace_back(0, type.GetID(), &testAsset);
    assets.emplace_back(1, type.GetID(), &testAsset);
    //保存资源
    jsonWriter.TransferField("assets", assets);
    //读取资源
    std::vector<Asset> newAssets = std::vector<Asset>(2);
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
            const Type& type = Type::GetType(asset.GetTypeID()).value().get();
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
    const Type& type = Type::GetType(typeid(CustomObject)).value();

    //写入文件
    std::ofstream outStream("test.bin", std::ios::binary);
    BinaryWriter binaryWriter = {outStream};
    type.Serialize(binaryWriter, &data);
    outStream.close();

    //读取文件
    std::ifstream inStream("test.bin", std::ios::binary);
    BinaryReader binaryReader = {inStream};
    CustomObject newData = {};
    type.Serialize(binaryReader, &newData);
    inStream.close();

    ASSERT_EQ(newData, data);
}

TEST(Reflection, JsonSerializer)
{
    const Type& type = Type::GetType(typeid(CustomObject)).value();

    rapidjson::Document document;
    document.Parse("{}");

    //写入文档
    JsonWriter jsonWriter = {document};
    type.Serialize(jsonWriter, &data);

    //读取文档
    JsonReader jsonReader = {document};
    CustomObject newData = {};
    type.Serialize(jsonReader, &newData);

    ASSERT_EQ(newData, data);
}