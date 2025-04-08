#include "System.h"

#include "GleamECS/Runtime/World/World.h"

namespace Gleam
{
    EntityAllocator& System::GetEntities() const
    {
        return world->GetEntityAllocator();
    }
}