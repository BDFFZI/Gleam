#include <fstream>
#include <gtest/gtest.h>
#include <rapidjson/stringbuffer.h>

#include "GleamPersistence/Runtime/JsonUtility.h"
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

    bool operator==(const TestAsset& other) const
    {
        return name == other.name && dependency == other.dependency;
    }
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

    ASSERT_EQ(static_cast<TestAsset*>(assets[0].GetObject())->name, static_cast<TestAsset*>(newAssets[0].GetObject())->name);
    ASSERT_EQ(static_cast<TestAsset*>(assets[1].GetObject())->name, static_cast<TestAsset*>(newAssets[1].GetObject())->name);
}


TEST(Reflection, AssetBundle)
{
    uuids::uuid assetBundleID = uuids::uuid::from_string("c57022f0-53a7-4b6b-99d5-41a1e5c8f51e").value();
    uuids::uuid assetBundle2ID = uuids::uuid::from_string("c492a4ff-b846-4596-8a8d-09e25cba9b08").value();

    //测试创建自依赖资源包
    {
        TestAsset testAsset[] = {
            {"Asset0"},
            {"Asset1"},
            {"Asset2"},
        };
        testAsset[0].dependency = &testAsset[1];
        testAsset[1].dependency = &testAsset[2];
        //验证创建资源包
        AssetBundle& assetBundle = AssetBundle::Create(assetBundleID);
        //验证添加资源
        assetBundle.AddAsset(&testAsset[0], TestAssetType);
        assetBundle.AddAsset(&testAsset[1], TestAssetType);
        //验证保存资源包
        AssetBundle::SaveJson("Assets/assetBundle.asset", assetBundle);
        AssetBundle::SaveBinary("Assets/" + to_string(assetBundle.GetID()), assetBundle);
        //验证卸载资源包
        AssetBundle::UnLoad(assetBundle);
    }

    //测试加载自依赖资源包
    {
        //验证json资源包id
        uuids::uuid uuid = AssetBundle::GetIDFromJson("Assets/assetBundle.asset");
        ASSERT_EQ(uuid, assetBundleID);
        //验证加载资源包
        AssetBundle& assetBundle = AssetBundle::LoadBinary("Assets/" + to_string(assetBundleID));
        ASSERT_EQ(assetBundle.GetAssets().size(), 3);
        TestAsset& asset0 = assetBundle.GetObject<TestAsset>(0);
        TestAsset& asset1 = assetBundle.GetObject<TestAsset>(1);
        TestAsset& asset2 = assetBundle.GetObject<TestAsset>(2);
        ASSERT_EQ(asset0.name, "Asset0");
        ASSERT_EQ(asset1.name, "Asset1");
        ASSERT_EQ(asset1.dependency, &asset2);
        AssetBundle::UnLoad(assetBundle);
    }

    //测试资源包分包的保存
    {
        TestAsset testAsset[] = {
            {"Asset0"},
            {"Asset1"},
            {"Asset2"},
            {"Asset3"},
        };

        //创建资源包2
        AssetBundle& assetBundle2 = AssetBundle::Create(assetBundle2ID);
        assetBundle2.AddAsset(&testAsset[3], TestAssetType);
        //迁移资源包1资源
        AssetBundle& assetBundle = AssetBundle::LoadBinary("Assets/" + to_string(assetBundleID));
        Asset asset = assetBundle.ExtractAsset(assetBundle.GetAssets()[0].GetID());
        assetBundle2.EmplaceAsset(std::move(asset));
        //保存资源包
        AssetBundle::SaveJson("Assets/assetBundle.asset", assetBundle);
        AssetBundle::SaveJson("Assets/assetBundle2.asset", assetBundle2);
        AssetBundle::UnLoad(assetBundle);
        AssetBundle::UnLoad(assetBundle2);
        //测试通过json间接还原二进制资源包
        AssetBundle::DumpJsonToBinary("Assets/assetBundle.asset", "Assets/" + to_string(assetBundleID), true);
        AssetBundle::DumpJsonToBinary("Assets/assetBundle2.asset", "Assets/" + to_string(assetBundle2ID), true);
    }

    //测试未加载依赖资源包时，引用丢失的现象
    {
        AssetBundle& assetBundle2 = AssetBundle::LoadBinary("Assets/" + to_string(assetBundle2ID));
        TestAsset* data3 = static_cast<TestAsset*>(assetBundle2.GetAssets()[1].GetObject())->dependency;
        ASSERT_EQ(data3, nullptr);
        AssetBundle::UnLoad(assetBundle2);
    }

    {
        //测试正确加载资源包后，获取到引用资源的现象
        AssetBundle& assetBundle = AssetBundle::LoadBinary("Assets/" + to_string(assetBundleID)); //资源包2依赖资源包1，必须加载，否则丢失引用
        AssetBundle& assetBundle2 = AssetBundle::LoadBinary("Assets/" + to_string(assetBundle2ID));
        TestAsset* data = static_cast<TestAsset*>(assetBundle2.GetAssets()[1].GetObject());
        ASSERT_EQ(data->name, "Asset0");
        ASSERT_EQ(data->dependency->name, "Asset1");

        //修改资源包内容，用于后续测试重载
        data->name += "Append";
        data->dependency->name += "Append";

        //重载资源包1并测试内容正确性
        AssetBundle::LoadBinary("Assets/" + to_string(assetBundleID), true);
        ASSERT_EQ(data->name, "Asset0Append");
        ASSERT_EQ(data->dependency->name, "Asset1");

        //重载资源包2并测试内容正确性
        AssetBundle::LoadBinary("Assets/" + to_string(assetBundle2ID), true);
        ASSERT_EQ(data->name, "Asset0");
        ASSERT_EQ(data->dependency->name, "Asset1");

        AssetBundle::UnLoad(assetBundle);
        AssetBundle::UnLoad(assetBundle2);
    }

    //测试多资源包引用未托管对象，保存时复制对象的现象
    {
        {
            AssetBundle& assetBundle = AssetBundle::LoadBinary("Assets/" + to_string(assetBundleID)); //资源包2依赖资源包1，必须加载，否则丢失引用
            AssetBundle& assetBundle2 = AssetBundle::LoadBinary("Assets/" + to_string(assetBundle2ID));
            TestAsset testAsset = {"Asset5", &assetBundle.GetObject<TestAsset>(1)};
            assetBundle.GetObject<TestAsset>(0).dependency = &testAsset;
            assetBundle2.GetObject<TestAsset>(0).dependency = &testAsset;
            AssetBundle::SaveJson("Assets/assetBundle.asset", assetBundle);
            AssetBundle::SaveJson("Assets/assetBundle2.asset", assetBundle2);
            AssetBundle::UnLoad(assetBundle);
            AssetBundle::UnLoad(assetBundle2);
        }

        {
            AssetBundle& assetBundle = AssetBundle::LoadJson("Assets/assetBundle.asset"); //资源包2依赖资源包1，必须加载，否则丢失引用
            AssetBundle& assetBundle2 = AssetBundle::LoadJson("Assets/assetBundle2.asset");
            ASSERT_EQ(assetBundle.GetObject<TestAsset>(2), assetBundle2.GetObject<TestAsset>(2));
            ASSERT_NE(&assetBundle.GetObject<TestAsset>(2), &assetBundle2.GetObject<TestAsset>(2));
        }
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

    std::cout << JsonUtility::DocumentToJson(document, true) << std::endl;

    //读取文档
    JsonReader jsonReader = {document};
    CustomObject newData = {};
    type.Serialize(jsonReader, &newData);

    ASSERT_EQ(newData, data);
}