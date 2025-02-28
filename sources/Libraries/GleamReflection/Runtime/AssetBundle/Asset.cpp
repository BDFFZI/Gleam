#include "Asset.h"

namespace Gleam
{
    Asset::Asset()
        : id(0), typeID({}), data(nullptr), ownership(false)
    {
    }
    Asset::Asset(const int id, const uuids::uuid type, void* data)
        : id(id), typeID(type), data(data), ownership(false)
    {
    }
    Asset::Asset(Asset&& asset) noexcept
    {
        id = asset.id;
        typeID = asset.typeID;
        data = asset.data;
        ownership = asset.ownership;
        asset.ownership = false;
    }
    Asset& Asset::operator=(Asset&& asset) noexcept
    {
        if (ownership)
        {
            Type::GetType(typeID).value().get().GetDestruct()(data);
            std::free(data);
        }

        id = asset.id;
        typeID = asset.typeID;
        data = asset.data;
        ownership = asset.ownership;
        asset.ownership = false;
        return *this;
    }
    Asset::~Asset()
    {
        if (ownership)
        {
            Type::GetType(typeID).value().get().GetDestruct()(data);
            std::free(data);
        }
    }
    uuids::uuid Asset::GetTypeID() const
    {
        return typeID;
    }
    void* Asset::GetData() const
    {
        return data;
    }
}