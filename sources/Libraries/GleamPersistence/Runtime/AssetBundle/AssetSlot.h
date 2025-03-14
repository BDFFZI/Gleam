#pragma once
#include "Asset.h"

namespace Gleam
{
    /**
     * 资源包内可存储多个资源，这些资源都被存放在资源槽内。
     *
     * 1. 资源槽具有ID，用于标识资源的持久化地址，其在单个资源包内是唯一的。
     * 2. 资源槽起到资源索引作用，因此其ID无法修改，但槽内的资源允许更换。
     * 3. 资源槽内的资源不许为空，因此只有在资源包中添加资源时，才会创建资源槽；移出资源时，资源槽就会被删除。
     */
    class AssetSlot
    {
    public:
        AssetSlot() = default;
        AssetSlot(int id, Asset&& asset);

        int GetID() const;
        const Asset& GetAsset() const;
        Asset& GetAsset();

    private:
        Gleam_MakeType_Friend
        friend class AssetBundle;

        int id;
        Asset asset;
    };

    Gleam_MakeType(AssetSlot, "1CEF16B4-DF11-41B0-A848-9221ABE9803B")
    {
        Gleam_MakeType_AddField(id);

        if constexpr (std::derived_from<TFieldTransferrer, FieldDataTransferrer>)
            AssetType.Serialize(transferrer, &value.asset);
        else
            Gleam_MakeType_AddField(asset);
    }
}