#include "Asset.h"

namespace Gleam
{
    Asset::Asset()
        : id(0), typeID({}), dataRef(nullptr), ownership(false)
    {
    }
    Asset::Asset(const int id, const uuids::uuid type, void* data)
        : id(id), typeID(type), dataRef(data), ownership(false)
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
            Type::GetType(typeID).value().get().GetDestruct()(dataRef);
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
            Type::GetType(typeID).value().get().GetDestruct()(dataRef);
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