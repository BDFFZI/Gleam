#include "AssetBundle.h"

#include <filesystem>
#include <fstream>
#include <regex>

#include "CustomTransferrer.h"
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
    void AssetBundle::Unload(AssetBundle& assetBundle, const bool releaseOwnership)
    {
        assert(HasInMemory(assetBundle.GetID()) && "内存中没有目标资源包！");

        assetBundle.ClearAssets(releaseOwnership);
        assetBundles.erase(assetBundle.id);
    }

    void AssetBundle::SaveBinary(const std::string_view fileName, AssetBundle& assetBundle)
    {
        assetBundle.AddAssetDependency();

        std::stringstream outStream;
        BinaryWriter binaryWriter = BinaryWriter(outStream);
        AssetBundleType.Serialize(binaryWriter, &assetBundle);
        File::WriteAllText(fileName, outStream.str());
    }
    void AssetBundle::SaveJson(const std::string_view fileName, AssetBundle& assetBundle)
    {
        assetBundle.AddAssetDependency();

        std::string json = JsonUtility::ToJson(&assetBundle, AssetBundleType, true);
        File::WriteAllText(fileName, json);
    }
    void AssetBundle::SaveMeta(const std::string_view fileName, AssetBundle& assetBundle)
    {
        //收集依赖信息
        AssetRefStatistician assetRefStatistician = {};
        AssetBundleType.Serialize(assetRefStatistician, &assetBundle);
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
        JsonUtility::FromJson(File::ReadAllText(jsonFile), AssetBundleType, &newAssetBundle);
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
            std::unordered_map<int, AssetSlot*> oldAssetSlots;
            for (auto& asset : oldAssetBundle.assetSlots)
                oldAssetSlots.emplace(asset.id, &asset);
            //更新资源
            for (auto& newAssetSlot : newAssetBundle.assetSlots)
            {
                if (oldAssetSlots.contains(newAssetSlot.id)) //内存中已有该资源，替换值
                {
                    Asset& oldAsset = oldAssetSlots[newAssetSlot.id]->GetAsset();
                    const Type& type = oldAsset.GetObjectType();
                    type.Move(oldAsset.GetObject(), newAssetSlot.GetAsset().GetObject());
                    oldAssetSlots.erase(newAssetSlot.id);
                }
                else //内存中没有，加入
                {
                    oldAssetBundle.EmplaceAsset(std::move(newAssetSlot));
                }
            }
            //去除内存中多余资源
            for (auto& assetSlot : oldAssetSlots | std::views::values)
                oldAssetBundle.RemoveAsset(assetSlot->GetAsset().GetObject());

            result = &oldAssetBundle;
        }
        else
        {
            //非重载状态，直接注册到内存中的资源包存储器即可
            newAssetBundle.BuildAssetIndex();
            result = &assetBundles.emplace(newAssetBundle.id, std::move(newAssetBundle)).first->second;
        }

        //依赖同资源包资源的指针，可能在依赖对象反持久化前被处理，导致无法获取依赖项的数据。
        //因此要在所有资源对象反序列化后重新序列化一次指针，利用上一次保存的指针与资源依赖的关系，重新连接资源。
        PointerSerializer pointerSerializer;
        AssetBundleType.Serialize(pointerSerializer, result);

        //清除临时保存的指针资源引用信息
        pointerToAssetSlot.clear();

        return *result;
    }
    AssetBundle& AssetBundle::LoadBinary(const std::string_view fileName, const bool reload)
    {
        assert(std::filesystem::exists(fileName) && "文件不存在！");

        //反序列化得到磁盘的中的资源包数据
        std::ifstream inStream(fileName.data(), std::ios::in | std::ios::binary);
        BinaryReader binaryReader = BinaryReader(inStream);
        AssetBundle newAssetBundle = {};
        AssetBundleType.Serialize(binaryReader, &newAssetBundle);

        return Load(newAssetBundle, reload);
    }
    AssetBundle& AssetBundle::LoadJson(const std::string_view fileName, const bool reload)
    {
        assert(std::filesystem::exists(fileName) && "文件不存在！");

        //反序列化得到磁盘的中的资源包数据
        AssetBundle newAssetBundle = {};
        JsonUtility::FromJson(File::ReadAllText(fileName), AssetBundleType, &newAssetBundle);

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
        if (objectToAssetSlot.contains(data))
            return objectToAssetSlot.at(data);
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
        auto result = assetBundle->GetAssetSlot(assetRef.assetID);
        if (result.has_value())
            return result.value().get().GetAsset().GetObject();

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
    const std::vector<AssetSlot>& AssetBundle::GetAssetSlots() const
    {
        return assetSlots;
    }
    std::optional<std::reference_wrapper<AssetSlot>> AssetBundle::GetAssetSlot(void* object)
    {
        auto it = std::ranges::find_if(assetSlots, [object](AssetSlot& assetSlot)
        {
            return assetSlot.GetAsset().GetObject() == object;
        });
        if (it != assetSlots.end())
            return *it;
        return std::nullopt;
    }
    std::optional<std::reference_wrapper<AssetSlot>> AssetBundle::GetAssetSlot(int slotID)
    {
        auto it = std::ranges::find_if(assetSlots, [slotID](const AssetSlot& asset)
        {
            return asset.id == slotID;
        });
        if (it != assetSlots.end())
            return *it;
        return std::nullopt;
    }
    Asset& AssetBundle::GetAsset(const int index)
    {
        return assetSlots[index].GetAsset();
    }

    void AssetBundle::AddAsset(Asset&& asset)
    {
        //添加资源
        EmplaceAsset(std::move(asset));
    }
    void AssetBundle::RemoveAsset(void* data)
    {
        //获取资源
        auto it = std::ranges::find_if(assetSlots, [data](AssetSlot& assetSlot)
        {
            return assetSlot.GetAsset().GetObject() == data;
        });
        auto index = it - assetSlots.begin();
        auto& asset = assetSlots[index];
        //移除索引信息
        objectToAssetSlot.erase(data);
        assetSlotToObject.erase(AssetRef{id, asset.id});
        assetSlotIDSet.erase(asset.id);
        //从内存中移除资源
        assetSlots.erase(assetSlots.begin() + index);
    }
    void AssetBundle::ClearAssets(const bool releaseOwnership)
    {
        if (releaseOwnership)
        {
            for (auto& assetSlot : assetSlots)
                assetSlot.GetAsset().SetOwnership(false);
        }

        for (auto& assetSlot : assetSlots)
        {
            //移除索引信息
            objectToAssetSlot.erase(assetSlot.GetAsset().GetObject());
            assetSlotToObject.erase(AssetRef{id, assetSlot.id});
        }

        assetSlotIDSet.clear();
        assetSlots.clear();
    }
    Asset AssetBundle::ExtractAsset(int slotID)
    {
        //提取资源槽
        auto it = std::ranges::find_if(assetSlots, [slotID](AssetSlot& asset)
        {
            return asset.id == slotID;
        });
        AssetSlot assetSlot = std::move(*it);
        assetSlots.erase(it);
        assetSlotIDSet.erase(assetSlot.id);

        //提取资源
        Asset asset = std::move(assetSlot.GetAsset());
        objectToAssetSlot.erase(asset.GetObject());
        assetSlotToObject.erase(AssetRef{id, assetSlot.id});

        return asset;
    }
    Asset& AssetBundle::EmplaceAsset(Asset&& asset, const std::optional<int> expectedSlotID)
    {
        assert(!objectToAssetSlot.contains(asset.GetObject()) && "资源已被添加到资源包！");
        assert(!assetSlotIDSet.contains(expectedSlotID.value_or(-1)) && "资源编号已被占用！");

        int slotID = expectedSlotID.value_or(GenerateAssetID());
        objectToAssetSlot.insert({asset.GetObject(), AssetRef{id, slotID}});
        assetSlotToObject.insert({AssetRef{id, slotID}, asset.GetObject()});
        assetSlotIDSet.insert(slotID);

        return assetSlots.emplace_back(slotID, std::move(asset)).GetAsset();
    }

    void AssetBundle::BuildAssetIndex()
    {
        for (auto& assetSlot : assetSlots)
        {
            assetSlotIDSet.insert(assetSlot.id);
            objectToAssetSlot.insert({assetSlot.GetAsset().GetObject(), AssetRef{id, assetSlot.id}});
            assetSlotToObject.insert({AssetRef{id, assetSlot.id}, assetSlot.GetAsset().GetObject()});
        }
    }
    int AssetBundle::GenerateAssetID() const
    {
        std::default_random_engine engine = std::default_random_engine(static_cast<uint32_t>(time(nullptr) + assetSlotIDSet.size()));
        std::uniform_int_distribution random = std::uniform_int_distribution(0, std::numeric_limits<int>::max());
        int assetID = random(engine);
        while (assetSlotIDSet.contains(assetID))
            assetID = random(engine);
        return assetID;
    }
    void AssetBundle::AddAssetDependency()
    {
        //获取依赖且未被资源包托管的对象
        ObjectRefStatistician pointerStatistician = {};
        AssetBundleType.Serialize(pointerStatistician, this);
        //将这些对象添加为本资源包的资源
        for (auto& asset : pointerStatistician.dependencies)
        {
            //优先复制资源
            void* duplicate = asset.GetObjectType().Create();
            asset.GetObjectType().Copy(duplicate, asset.GetObject());
            AddAsset(Asset{duplicate, asset.GetObjectType(), true});
            //无法复制则尝试移动
            //TODO
            //修改使用者的指针引用
            for (auto& user : pointerStatistician.dependencyUsers[asset.GetObject()])
                *user = duplicate;
        }
    }
}