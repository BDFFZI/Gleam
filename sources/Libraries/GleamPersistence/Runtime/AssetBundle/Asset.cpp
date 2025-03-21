#include "Asset.h"

namespace Gleam
{
    Asset::Asset(const std::shared_ptr<void>& object, const Type& objectType)
        : objectPtr(object), objectType(&objectType)
    {
    }
    Asset::Asset(Asset&& other) noexcept
    {
        objectType = other.objectType;
        objectPtr = other.objectPtr;
        other.objectPtr.reset();
        other.objectType = nullptr;
    }
    Asset& Asset::operator=(Asset&& other) noexcept
    {
        objectType = other.objectType;
        objectPtr = other.objectPtr;
        other.objectPtr.reset();
        other.objectType = nullptr;
        return *this;
    }

    std::shared_ptr<void>& Asset::GetObjectPtr()
    {
        return objectPtr;
    }
    const Type& Asset::GetObjectType() const
    {
        return *objectType;
    }
}