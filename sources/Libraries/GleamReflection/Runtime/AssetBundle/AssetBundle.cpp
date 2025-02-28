#include "AssetBundle.h"

#include <filesystem>
#include <fstream>
#include <regex>

#include "GleamMath/Runtime/Random.h"
#include "GleamReflection/Runtime/Serialization/BinaryReader.h"
#include "GleamReflection/Runtime/Serialization/BinaryWriter.h"
#include "GleamReflection/Runtime/Serialization/JsonReader.h"
#include "GleamReflection/Runtime/Serialization/JsonUtility.h"
#include "GleamUtility/Runtime/File.h"

namespace Gleam
{
    AssetBundle& AssetBundle::Create(uuids::uuid assetBundleID)
    {
        if (assetBundleID.is_nil())
        {
            auto generator = uuids::uuid_system_generator();
            assetBundleID = generator();
        }

        std::unique_ptr<AssetBundle> assetBundle = std::make_unique<AssetBundle>();
        assetBundle->id = assetBundleID;
        return *assetBundles.insert({assetBundleID, std::move(assetBundle)}).first->second;
    }
    void AssetBundle::Destroy(AssetBundle& assetBundle, const bool retainAssets)
    {
        if (retainAssets)
        {
            for (auto& asset : assetBundle.assets)
                asset.ownership = false;
        }

        assetBundle.ClearAssets(); //清除索引信息
        assetBundles.erase(assetBundle.id);
    }
    bool AssetBundle::Exists(const uuids::uuid assetBundleID)
    {
        return std::filesystem::exists(assetBundleDirectory + to_string(assetBundleID));
    }
    void AssetBundle::Save(AssetBundle& assetBundle)
    {
        std::stringstream outStream;
        BinaryWriter binaryWriter = BinaryWriter(outStream);

        AssetBundleType.GetSerialize()(binaryWriter, &assetBundle);
        File::WriteAllText(assetBundleDirectory + to_string(assetBundle.id), outStream.str());
    }
    AssetBundle& AssetBundle::Load(const uuids::uuid assetBundleID)
    {
        assert(Exists(assetBundleID) && "资源包不存在！");
        std::ifstream inStream(assetBundleDirectory + to_string(assetBundleID), std::ios::in | std::ios::binary);
        BinaryReader binaryReader = BinaryReader(inStream);

        std::unique_ptr<AssetBundle> assetBundle = std::make_unique<AssetBundle>();
        AssetBundleType.GetSerialize()(binaryReader, assetBundle.get());
        assetBundle->RefreshSet();
        return *assetBundles.insert({assetBundleID, std::move(assetBundle)}).first->second;
    }

    uuids::uuid AssetBundle::GetIDFromJson(const std::string_view fileName)
    {
        std::ifstream inStream(fileName.data(), std::ios::in | std::ios::binary);
        std::string json;
        json.resize(60);
        inStream.read(json.data(), static_cast<std::streamsize>(json.size()));
        inStream.close();

        std::match_results<std::string::const_iterator> result;
        std::regex_match(json, result, std::regex(R"#([\s\S]*?"id": "(.*?)"[\s\S]*)#"));
        return uuids::uuid::from_string(result[1].str()).value();
    }
    void AssetBundle::SaveJson(const std::string_view fileName, AssetBundle& assetBundle, const bool saveBinary)
    {
        rapidjson::Document document;
        document.Parse("{}");
        JsonWriter jsonWriter = JsonWriter(document);
        AssetBundleType.GetSerialize()(jsonWriter, &assetBundle);
        std::string json = JsonUtility::DocumentToJson(document, true);
        File::WriteAllText(fileName, json);

        if (saveBinary)
            Save(assetBundle);
    }
    AssetBundle& AssetBundle::LoadJson(const std::string_view fileName)
    {
        rapidjson::Document document;
        document.Parse(File::ReadAllText(fileName).c_str());
        JsonReader jsonReader = JsonReader(document);

        std::unique_ptr<AssetBundle> assetBundle = std::make_unique<AssetBundle>();
        AssetBundleType.GetSerialize()(jsonReader, assetBundle.get());
        assetBundle->RefreshSet();
        return *assetBundles.insert({assetBundle->id, std::move(assetBundle)}).first->second;
    }
    void AssetBundle::DumpJson(const std::string_view fileName)
    {
        Save(LoadJson(fileName));
    }
    std::optional<void*> AssetBundle::Load(AssetRef assetRef)
    {
        //加载资源包
        if (!Exists(assetRef.assetBundleID))
            return std::nullopt;
        AssetBundle& assetBundle = Load(assetRef.assetBundleID);

        //获取资源
        auto it = std::ranges::find_if(
            assetBundle.assets,
            [assetID = assetRef.assetID](const auto& asset) { return asset.id == assetID; }
        );
        if (it == assetBundle.assets.end())
            return std::nullopt;

        return it->data;
    }

    void AssetBundle::AddAsset(void* data, Type& dataType)
    {
        //创建资源ID
        std::default_random_engine engine = std::default_random_engine(static_cast<uint32_t>(time(nullptr)));
        std::uniform_int_distribution random = std::uniform_int_distribution(0, std::numeric_limits<int>::max());
        int assetID = random(engine);
        while (!assetIDSet.emplace(assetID).second)
            assetID = random(engine);
        //添加索引信息
        dataToAsset.insert({data, AssetRef{id, assetID}});
        assetToData.insert({AssetRef{id, assetID}, data});
        //添加资源
        assets.emplace_back(assetID, dataType.GetID(), data);
    }
    void AssetBundle::RemoveAsset(void* data)
    {
        //获取资源
        auto it = std::ranges::find_if(assets, [data](Asset& asset) { return asset.data == data; });
        auto index = it - assets.begin();
        auto& asset = assets[index];
        //移除索引信息
        dataToAsset.erase(data);
        assetToData.erase(AssetRef{id, asset.id});
        assetIDSet.erase(asset.id);
        //从内存中移除资源
        assets.erase(assets.begin() + index);
    }
    void AssetBundle::ClearAssets()
    {
        for (auto& asset : assets)
        {
            //移除索引信息
            dataToAsset.erase(asset.data);
            assetToData.erase(AssetRef{id, asset.id});
        }

        assetIDSet.clear();
        assets.clear();
    }
    uuids::uuid AssetBundle::GetID() const
    {
        return id;
    }
    const std::vector<Asset>& AssetBundle::GetAssets()
    {
        return assets;
    }
    void AssetBundle::RefreshSet()
    {
        assetIDSet.clear();
        for (auto& assetBundle : assets)
            assetIDSet.insert(assetBundle.id);
    }
}