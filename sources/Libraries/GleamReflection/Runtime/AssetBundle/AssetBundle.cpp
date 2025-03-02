#include "AssetBundle.h"

#include <filesystem>
#include <fstream>
#include <regex>

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

        assert(!HasInMemory(assetBundleID) && "内存中已有目标资源包！");

        AssetBundle assetBundle = {};
        assetBundle.id = assetBundleID;
        return assetBundles.insert({assetBundleID, std::move(assetBundle)}).first->second;
    }
    void AssetBundle::Save(AssetBundle& assetBundle)
    {
        std::stringstream outStream;
        BinaryWriter binaryWriter = BinaryWriter(outStream);

        AssetBundleType.GetSerialize()(binaryWriter, &assetBundle);
        File::WriteAllText(assetBundleDirectory + to_string(assetBundle.id), outStream.str());
    }
    AssetBundle& AssetBundle::Load(AssetBundle& newAssetBundle, const bool reload)
    {
        AssetBundle& assetBundleSlot = assetBundles[newAssetBundle.id];
        if (reload) //重载
        {
            //统计目前内存中的资源
            AssetBundle& oldAssetBundle = assetBundleSlot;
            std::unordered_map<int, Asset*> oldAssets;
            for (auto& asset : oldAssetBundle.assets)
                oldAssets.emplace(asset.id, &asset);
            //更新资源
            for (auto& asset : newAssetBundle.assets)
            {
                if (oldAssets.contains(asset.id)) //内存中已有该资源，替换值
                {
                    Asset* oldAsset = oldAssets[asset.id];
                    Type& type = Type::GetType(oldAsset->GetTypeID()).value().get();
                    type.GetMove()(oldAsset->dataRef, asset.dataRef);
                    oldAssets.erase(asset.id);
                }
                else //内存中没有，加入
                {
                    oldAssetBundle.EmplaceAsset(std::move(asset));
                }
            }
            //去除内存中多余资源
            for (auto& [id,asset] : oldAssets)
                oldAssetBundle.RemoveAsset(asset->dataRef);
        }
        else
        {
            //非重载状态，直接注册到内存中的资源包存储器即可
            newAssetBundle.BuildAssetIndex();
            assetBundleSlot = std::move(newAssetBundle);
        }

        //资源包所有资源加载完毕后需二次同步指针，以解决同资源包的资源依赖，因加载顺序原因导致丢失引用的问题
        PointerTransferrer pointerTransferrer = {};
        AssetBundleType.GetSerialize()(pointerTransferrer, &assetBundleSlot);

        return assetBundleSlot;
    }
    AssetBundle& AssetBundle::Load(const uuids::uuid assetBundleID, const bool reload)
    {
        assert(HasInDisk(assetBundleID) && "磁盘中没有目标资源包！");
        assert(reload || (!HasInMemory(assetBundleID) && "内存中已有目标资源包！"));

        //提前占用空间，这样可以通过断言检测循环加载
        assetBundles.try_emplace(assetBundleID);

        //反序列化得到磁盘的中的资源包数据
        std::ifstream inStream(assetBundleDirectory + to_string(assetBundleID), std::ios::in | std::ios::binary);
        BinaryReader binaryReader = BinaryReader(inStream);
        AssetBundle newAssetBundle = {};
        AssetBundleType.GetSerialize()(binaryReader, &newAssetBundle);

        return Load(newAssetBundle, reload);
    }
    void AssetBundle::UnLoad(AssetBundle& assetBundle, const bool retainAssets)
    {
        assert(HasInMemory(assetBundle.GetID()) && "内存中没有目标资源包！");

        if (retainAssets)
        {
            for (auto& asset : assetBundle.assets)
                asset.ownership = false;
        }

        assetBundle.ClearAssets(); //清除索引信息
        assetBundles.erase(assetBundle.id);
    }

    std::optional<AssetRef> AssetBundle::GetAssetRef(void* data)
    {
        if (dataToAsset.contains(data))
            return dataToAsset.at(data);
        return std::nullopt;
    }
    std::optional<void*> AssetBundle::GetDataRef(const AssetRef& assetRef)
    {
        AssetBundle* assetBundle;
        if (HasInMemory(assetRef.assetBundleID))
            assetBundle = &assetBundles[assetRef.assetBundleID]; //尝试从内存中获取资源包
        else if (HasInDisk(assetRef.assetBundleID))
            assetBundle = &Load(assetRef.assetBundleID); //尝试从磁盘中获取资源包
        else
            return std::nullopt;

        //获取资源包中的资源
        auto result = assetBundle->GetAsset(assetRef.assetID);
        if (result.has_value())
            return result.value().get().dataRef;

        return std::nullopt;
    }
    AssetBundle& AssetBundle::GetAssetBundle(const uuids::uuid assetBundleID)
    {
        return assetBundles.at(assetBundleID);
    }
    bool AssetBundle::HasInDisk(const uuids::uuid assetBundleID)
    {
        return std::filesystem::exists(assetBundleDirectory + to_string(assetBundleID));
    }
    bool AssetBundle::HasInMemory(const uuids::uuid assetBundleID)
    {
        return assetBundles.contains(assetBundleID);
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
        auto optionalUuid = uuids::uuid::from_string(result[1].str());
        if (!optionalUuid.has_value())
            throw std::runtime_error("资源包文件内容异常！");
        return optionalUuid.value();
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
    AssetBundle& AssetBundle::LoadJson(std::string_view fileName, bool reload)
    {
        uuids::uuid assetBundleID = GetIDFromJson(fileName);
        assert(!HasInMemory(assetBundleID) && "内存中已有目标资源包！");

        //提前占用空间，这样可以通过断言检测循环加载
        assetBundles.try_emplace(assetBundleID);

        //反序列化得到磁盘的中的资源包数据
        rapidjson::Document document;
        document.Parse(File::ReadAllText(fileName).c_str());
        JsonReader jsonReader = JsonReader(document);
        AssetBundle newAssetBundle = {};
        AssetBundleType.GetSerialize()(jsonReader, &newAssetBundle);

        return Load(newAssetBundle, reload);
    }
    void AssetBundle::DumpJson(const std::string_view fileName)
    {
        //反序列化得到json中的资源包数据
        rapidjson::Document document;
        document.Parse(File::ReadAllText(fileName).c_str());
        JsonReader jsonReader = JsonReader(document);
        AssetBundle newAssetBundle = {};
        AssetBundleType.GetSerialize()(jsonReader, &newAssetBundle);
        //转存为二进制文件
        Save(newAssetBundle);
    }

    uuids::uuid AssetBundle::GetID() const
    {
        return id;
    }
    const std::vector<Asset>& AssetBundle::GetAssets() const
    {
        return assets;
    }
    std::optional<std::reference_wrapper<const Asset>> AssetBundle::GetAsset(int assetID) const
    {
        const auto it = std::ranges::find_if(assets, [assetID](const Asset& asset) { return asset.id == assetID; });
        return it == assets.end() ? std::nullopt : std::optional<std::reference_wrapper<const Asset>>{*it};
    }

    void AssetBundle::AddAsset(void* data, const Type& dataType)
    {
        //创建资源ID
        std::default_random_engine engine = std::default_random_engine(static_cast<uint32_t>(time(nullptr)));
        std::uniform_int_distribution random = std::uniform_int_distribution(0, std::numeric_limits<int>::max());
        int assetID = random(engine);
        while (assetIDSet.contains(assetID))
            assetID = random(engine);
        //添加资源
        Asset asset = {assetID, dataType.GetID(), data};
        EmplaceAsset(std::move(asset));
    }
    void AssetBundle::RemoveAsset(void* data)
    {
        //获取资源
        auto it = std::ranges::find_if(assets, [data](Asset& asset) { return asset.dataRef == data; });
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
            dataToAsset.erase(asset.dataRef);
            assetToData.erase(AssetRef{id, asset.id});
        }

        assetIDSet.clear();
        assets.clear();
    }
    void AssetBundle::EmplaceAsset(Asset&& asset)
    {
        assert(!dataToAsset.contains(asset.dataRef) && "资源已被添加到资源包！");

        dataToAsset.insert({asset.dataRef, AssetRef{id, asset.id}});
        assetToData.insert({AssetRef{id, asset.id}, asset.dataRef});
        assetIDSet.insert(asset.id);
        assets.emplace_back(std::move(asset));
    }
    Asset AssetBundle::ExtractAsset(int assetID)
    {
        auto it = std::ranges::find_if(assets, [assetID](Asset& asset) { return asset.id == assetID; });
        Asset asset = std::move(*it);

        dataToAsset.erase(asset.dataRef);
        assetToData.erase(AssetRef{id, asset.id});
        assetIDSet.erase(asset.id);
        assets.erase(it);

        return asset;
    }

    void AssetBundle::BuildAssetIndex()
    {
        for (auto& assetBundle : assets)
        {
            assetIDSet.insert(assetBundle.id);
            dataToAsset.insert({assetBundle.dataRef, AssetRef{id, assetBundle.id}});
            assetToData.insert({AssetRef{id, assetBundle.id}, assetBundle.dataRef});
        }
    }
}