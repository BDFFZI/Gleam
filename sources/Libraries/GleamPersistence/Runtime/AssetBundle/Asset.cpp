#include "Asset.h"

namespace Gleam
{
    Asset::Asset()
        : id(-1), typeID({}), object(nullptr), ownership(false)
    {
    }
    Asset::Asset(const int id, const uuids::uuid typeID, void* object, const bool ownership)
        : id(id), typeID(typeID), object(object), ownership(ownership)
    {
    }
    Asset::Asset(Asset&& asset) noexcept
    {
        id = asset.id;
        typeID = asset.typeID;
        object = asset.object;
        ownership = asset.ownership;
        asset.ownership = false;
    }
    Asset& Asset::operator=(Asset&& asset) noexcept
    {
        if (ownership)
        {
            Type::GetType(typeID).value().get().Destruct(object);
            std::free(object);
        }

        id = asset.id;
        typeID = asset.typeID;
        object = asset.object;
        ownership = asset.ownership;
        asset.ownership = false;
        return *this;
    }
    Asset::~Asset()
    {
        if (ownership)
        {
            Type::GetType(typeID).value().get().Destroy(object);
        }
    }

    int Asset::GetID() const
    {
        return id;
    }
    uuids::uuid Asset::GetTypeID() const
    {
        return typeID;
    }
    void* Asset::GetObject() const
    {
        return object;
    }
}