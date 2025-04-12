#pragma once
#include "ForceSystem.h"
#include "GleamECS/Runtime/System/SystemGroup.h"

namespace Gleam
{
    class CollisionSystem : public System
    {
    public:
        CollisionSystem(): System(GlobalForceSystem, SystemRelation::After)
        {
        }

    private:
        Gleam_MakeType_Friend

        int ccdMaxCount = 3;

        void Update() override;
    };
    Gleam_MakeTypeWithID(CollisionSystem, "")
    {
        Gleam_MakeType_AddField(ccdMaxCount);
    }

    Gleam_MakeRuntimeSystem(CollisionSystem)
}