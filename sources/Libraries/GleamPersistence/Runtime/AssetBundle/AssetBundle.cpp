#include "AssetBundle.h"

#include <filesystem>
#include <fstream>
#include <regex>

#include "AssetRefTransferrer.h"
#include "GleamPersistence/Runtime/Serializer/BinaryReader.h"
#include "GleamPersistence/Runtime/Serializer/BinaryWriter.h"
#include "GleamPersistence/Runtime/Serializer/JsonReader.h"
#include "GleamPersistence/Runtime/JsonUtility.h"
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
    
    void AssetBundle::SaveBinary(const std::string_view fileName, AssetBundle& assetBundle)
    {
        assetBundle.AddAssetDependency();

        std::stringstream outStream;
        BinaryWriter binaryWriter = BinaryWriter(outStream);
        FieldDataTransferrer_TransferPtrEvent = SerializePtr;
        AssetBundleType.Serialize(binaryWriter, &assetBundle);
        FieldDataTransferrer_TransferPtrEvent = nullptr;
        File::WriteAllText(fileName, outStream.str());
    }
    void AssetBundle::SaveJson(const std::string_view fileName, AssetBundle& assetBundle)
    {
        assetBundle.AddAssetDependency();

        FieldDataTransferrer_TransferPtrEvent = SerializePtr;
        std::string json = JsonUtility::ToJson(&assetBundle, AssetBundleType, true);
        FieldDataTransferrer_TransferPtrEvent = nullptr;
        File::WriteAllText(fileName, json);
    }
    void AssetBundle::SaveMeta(const std::string_view fileName, AssetBundle& assetBundle)
    {
        //收集依赖信息
        FieldDataTransferrer_TransferPtrEvent = SerializePtr;
        AssetRefStatistician assetRefStatistician = {};
        AssetBundleType.Serialize(assetRefStatistician, &assetBundle);
        FieldDataTransferrer_TransferPtrEvent = nullptr;
        //填充资源包元信息
        AssetBundleMeta assetBundleMeta;
        for (auto& assetRef : assetRefStatistician.result)
        {
            if (assetRef.assetBundleID != assetBundle.id)
                assetBundleMeta.dependencies.push_back(assetRef.assetBundleID);
        }
        //保存元信息
        std::string meta = JsonUtility::ToJson(&assetBundleMeta, AssetBundleMetaType, true);
        File::WriteAllText(std::string(fileName) + ".meta", meta);
    }
    void AssetBundle::DumpJsonToBinary(const std::string_view jsonFile, const std::string_view binaryFile, const bool saveMeta)
    {
        //反序列化得到json中的资源包数据
        AssetBundle newAssetBundle = {};
        FieldDataTransferrer_TransferPtrEvent = SerializePtr;
        JsonUtility::FromJson(File::ReadAllText(jsonFile), AssetBundleType, &newAssetBundle);
        FieldDataTransferrer_TransferPtrEvent = nullptr;
        //转存为二进制文件
        SaveBinary(binaryFile, newAssetBundle);
        //保存meta信息
        if (saveMeta)
            SaveMeta(binaryFile, newAssetBundle);
    }

    AssetBundle& AssetBundle::Load(AssetBundle& newAssetBundle, const bool reload)
    {
        assert(reload || (!HasInMemory(newAssetBundle.id) && "内存中已有目标资源包！"));

        AssetBundle* result;
        if (reload && HasInMemory(newAssetBundle.id)) //重载
        {
            AssetBundle& oldAssetBundle = assetBundles.at(newAssetBundle.id);
            //统计目前内存中的资源
            std::unordered_map<int, Asset*> oldAssets;
            for (auto& asset : oldAssetBundle.assets)
                oldAssets.emplace(asset.id, &asset);
            //更新资源
            for (auto& asset : newAssetBundle.assets)
            {
                if (oldAssets.contains(asset.id)) //内存中已有该资源，替换值
                {
                    Asset* oldAsset = oldAssets[asset.id];
                    const Type& type = Type::GetType(oldAsset->GetTypeID()).value().get();
                    type.Move(oldAsset->object, asset.object);
                    oldAssets.erase(asset.id);
                }
                else //内存中没有，加入
                {
                    oldAssetBundle.EmplaceAsset(std::move(asset));
                }
            }
            //去除内存中多余资源
            for (auto& [id,asset] : oldAssets)
                oldAssetBundle.RemoveAsset(asset->object);

            result = &oldAssetBundle;
        }
        else
        {
            //非重载状态，直接注册到内存中的资源包存储器即可
            newAssetBundle.BuildAssetIndex();
            result = &assetBundles.emplace(newAssetBundle.id, std::move(newAssetBundle)).first->second;
        }

        //依赖同资源包资源的指针，可能在依赖对象反资源化前被处理，导致无法获取依赖项的数据。
        //因此要在所有资源对象反序列化后重新资源化一次指针，利用上一次保存的指针与资源依赖的关系，重新连接资源。
        FieldDataTransferrer_TransferPtrEvent = SerializePtr;
        NullTransferrer pointerTransferrer = {};
        AssetBundleType.Serialize(pointerTransferrer, result);
        FieldDataTransferrer_TransferPtrEvent = nullptr;

        return *result;
    }
    AssetBundle& AssetBundle::LoadBinary(const std::string_view fileName, const bool reload)
    {
        assert(std::filesystem::exists(fileName) && "文件不存在！");

        //反序列化得到磁盘的中的资源包数据
        std::ifstream inStream(fileName.data(), std::ios::in | std::ios::binary);
        BinaryReader binaryReader = BinaryReader(inStream);
        AssetBundle newAssetBundle = {};
        FieldDataTransferrer_TransferPtrEvent = SerializePtr;
        AssetBundleType.Serialize(binaryReader, &newAssetBundle);
        FieldDataTransferrer_TransferPtrEvent = nullptr;

        return Load(newAssetBundle, reload);
    }
    AssetBundle& AssetBundle::LoadJson(const std::string_view fileName, const bool reload)
    {
        assert(std::filesystem::exists(fileName) && "文件不存在！");

        //反序列化得到磁盘的中的资源包数据
        AssetBundle newAssetBundle = {};
        FieldDataTransferrer_TransferPtrEvent = SerializePtr;
        JsonUtility::FromJson(File::ReadAllText(fileName), AssetBundleType, &newAssetBundle);
        FieldDataTransferrer_TransferPtrEvent = nullptr;

        return Load(newAssetBundle, reload);
    }
    AssetBundleMeta AssetBundle::LoadMeta(const std::string_view fileName)
    {
        std::string json = File::ReadAllText(std::string(fileName) + ".meta");

        AssetBundleMeta assetBundleMeta;
        JsonUtility::FromJson(json, AssetBundleMetaType, &assetBundleMeta);
        return assetBundleMeta;
    }



    std::optional<AssetRef> AssetBundle::GetAssetRef(void* data)
    {
        if (dataToAsset.contains(data))
            return dataToAsset.at(data);
        return std::nullopt;
    }
    std::optional<void*> AssetBundle::GetObject(const AssetRef& assetRef)
    {
        AssetBundle* assetBundle;
        if (HasInMemory(assetRef.assetBundleID))
            assetBundle = &assetBundles[assetRef.assetBundleID];
        else
            return std::nullopt;

        //获取资源包中的资源
        auto result = assetBundle->GetAssetFromID(assetRef.assetID);
        if (result.has_value())
            return result.value().get().object;

        return std::nullopt;
    }
    AssetBundle& AssetBundle::GetAssetBundle(const uuids::uuid assetBundleID)
    {
        return assetBundles.at(assetBundleID);
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

    uuids::uuid AssetBundle::GetID() const
    {
        return id;
    }
    const Asset& AssetBundle::GetAsset(const int index) const
    {
        return assets[index];
    }
    std::optional<std::reference_wrapper<const Asset>> AssetBundle::GetAsset(void* data)
    {
        auto it = std::ranges::find_if(assets, [data](Asset& asset) { return asset.object == data; });
        if (it != assets.end())
            return *it;
        return std::nullopt;
    }
    const std::vector<Asset>& AssetBundle::GetAssets() const
    {
        return assets;
    }
    std::optional<std::reference_wrapper<const Asset>> AssetBundle::GetAssetFromID(int assetID) const
    {
        const auto it = std::ranges::find_if(assets, [assetID](const Asset& asset) { return asset.id == assetID; });
        return it == assets.end() ? std::nullopt : std::optional<std::reference_wrapper<const Asset>>{*it};
    }

    void AssetBundle::AddAsset(void* data, const Type& dataType, const bool ownership)
    {
        //添加资源
        Asset asset = {GenerateAssetID(), dataType.GetID(), data, ownership};
        EmplaceAsset(std::move(asset));
    }
    void AssetBundle::RemoveAsset(void* data)
    {
        //获取资源
        auto it = std::ranges::find_if(assets, [data](Asset& asset) { return asset.object == data; });
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
            dataToAsset.erase(asset.object);
            assetToData.erase(AssetRef{id, asset.id});
        }

        assetIDSet.clear();
        assets.clear();
    }
    Asset& AssetBundle::EmplaceAsset(Asset&& asset)
    {
        assert(!dataToAsset.contains(asset.object) && "资源已被添加到资源包！");

        asset.id = GenerateAssetID(); //不同资源包内的资源ID可能重复，故需要重新生成
        dataToAsset.insert({asset.object, AssetRef{id, asset.id}});
        assetToData.insert({AssetRef{id, asset.id}, asset.object});
        assetIDSet.insert(asset.id);
        return assets.emplace_back(std::move(asset));
    }
    Asset AssetBundle::ExtractAsset(int assetID)
    {
        auto it = std::ranges::find_if(assets, [assetID](Asset& asset) { return asset.id == assetID; });
        Asset asset = std::move(*it);

        dataToAsset.erase(asset.object);
        assetToData.erase(AssetRef{id, asset.id});
        assetIDSet.erase(asset.id);
        assets.erase(it);

        return asset;
    }

    void AssetBundle::SerializePtr(FieldDataTransferrer& serializer, void*& value, std::type_index)
    {
        AssetRef assetRef = pointerMapping[reinterpret_cast<uintptr_t>(&value)];
        {
            assetRef = GetAssetRef(value).value_or(assetRef); //获取引用数据对应的资源依赖
            assert(value == nullptr || !assetRef.assetBundleID.is_nil() && "指针引用的物体未被资源化！");
            serializer.Transfer(assetRef);
            value = GetObject(assetRef).value_or(nullptr); //根据资源依赖获取数据
        }
        pointerMapping[reinterpret_cast<uintptr_t>(&value)] = assetRef;
    }

    void AssetBundle::BuildAssetIndex()
    {
        for (auto& assetBundle : assets)
        {
            assetIDSet.insert(assetBundle.id);
            dataToAsset.insert({assetBundle.object, AssetRef{id, assetBundle.id}});
            assetToData.insert({AssetRef{id, assetBundle.id}, assetBundle.object});
        }
    }
    int AssetBundle::GenerateAssetID() const
    {
        std::default_random_engine engine = std::default_random_engine(static_cast<uint32_t>(time(nullptr) + assetIDSet.size()));
        std::uniform_int_distribution random = std::uniform_int_distribution(0, std::numeric_limits<int>::max());
        int assetID = random(engine);
        while (assetIDSet.contains(assetID))
            assetID = random(engine);
        return assetID;
    }
    void AssetBundle::AddAssetDependency()
    {
        //获取依赖且未被资源包托管的对象
        std::vector<std::tuple<void*, const Type*>> dependencies; //未托管对象
        std::unordered_map<void*, std::vector<void**>> users; //使用对象的指针
        NullTransferrer nullTransferrer = {};
        FieldDataTransferrer_TransferPtrEvent = [&dependencies,&users](FieldDataTransferrer&, void*& value, const std::type_index typeIndex)
        {
            if (value == nullptr)
                return;

            std::optional<std::reference_wrapper<const Type>> optionalType = Type::GetType(typeIndex);
            if (optionalType.has_value() && !dataToAsset.contains(value))
            {
                dependencies.emplace_back(value, &optionalType.value().get());
                users[value].push_back(&value);
            }
        };
        AssetBundleType.Serialize(nullTransferrer, this);
        FieldDataTransferrer_TransferPtrEvent = nullptr;
        //将这些对象添加为本资源包的资源
        for (const auto& [object,objectType] : dependencies)
        {
            void* duplicate = objectType->Create();
            objectType->Copy(duplicate, object);
            AddAsset(duplicate, *objectType, true);
            //修改使用者的指针引用
            for (auto& user : users[object])
                *user = duplicate;
        }
    }
}