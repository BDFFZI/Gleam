#include "Asset.h"

namespace Gleam
{
    Asset::Asset(void* object, const Type& objectType, const bool ownership)
        : object(object), objectType(&objectType), ownership(ownership)
    {
    }
    Asset::Asset(Asset&& asset) noexcept
    {
        objectType = asset.objectType;
        object = asset.object;
        ownership = asset.ownership;
        asset.ownership = false;
    }
    Asset& Asset::operator=(Asset&& asset) noexcept
    {
        if (ownership)
        {
            objectType->Destroy(object);
        }

        objectType = asset.objectType;
        object = asset.object;
        ownership = asset.ownership;
        asset.ownership = false;
        return *this;
    }
    Asset::~Asset()
    {
        if (ownership)
        {
            objectType->Destroy(object);
        }
    }

    void* Asset::GetObject() const
    {
        return object;
    }
    const Type& Asset::GetObjectType() const
    {
        return *objectType;
    }
    bool Asset::GetOwnership() const
    {
        return ownership;
    }
    void Asset::SetOwnership(const bool ownership)
    {
        this->ownership = ownership;
    }
}