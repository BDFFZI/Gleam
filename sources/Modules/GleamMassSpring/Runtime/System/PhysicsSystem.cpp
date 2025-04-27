#include "PhysicsSystem.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"

namespace Gleam
{
    void PhysicsSystem::Update()
    {
        int fixedDeltaCount = World::GetSystemAllocator().GetSystem<TimeSystem>().GetFixedDeltaCount();
        for (int i = 0; i < fixedDeltaCount; i++)
            ISystemGroup::Update();
    }
}