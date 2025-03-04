#include "Asset.h"

namespace Gleam
{
    Asset::Asset()
        : id(-1), typeID({}), dataRef(nullptr), ownership(false)
    {
    }
    Asset::Asset(const int id, const uuids::uuid typeID, void* dataRef)
        : id(id), typeID(typeID), dataRef(dataRef), ownership(false)
    {
    }
    Asset::Asset(Asset&& asset) noexcept
    {
        id = asset.id;
        typeID = asset.typeID;
        dataRef = asset.dataRef;
        ownership = asset.ownership;
        asset.ownership = false;
    }
    Asset& Asset::operator=(Asset&& asset) noexcept
    {
        if (ownership)
        {
            Type::GetType(typeID).value().get().Destruct(dataRef);
            std::free(dataRef);
        }

        id = asset.id;
        typeID = asset.typeID;
        dataRef = asset.dataRef;
        ownership = asset.ownership;
        asset.ownership = false;
        return *this;
    }
    Asset::~Asset()
    {
        if (ownership)
        {
            Type::GetType(typeID).value().get().Destruct(dataRef);
            std::free(dataRef);
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
    void* Asset::GetDataRef() const
    {
        return dataRef;
    }
}