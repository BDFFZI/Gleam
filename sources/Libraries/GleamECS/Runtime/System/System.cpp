#include "System.h"

#include "GleamECS/Runtime/World/World.h"

namespace Gleam
{
    EntityAllocator& System::GetAllocator() const
    {
        return world->GetEntityAllocator();
    }
}