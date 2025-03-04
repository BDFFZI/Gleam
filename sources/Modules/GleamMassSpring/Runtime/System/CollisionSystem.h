#pragma once
#include "ForceSystem.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class CollisionSystem : public System
    {
    public:
        CollisionSystem(): System(GlobalForceSystem, OrderRelation::After)
        {
        }

    private:
        Gleam_MakeType_Friend

        int ccdMaxCount = 3;

        void Update() override;
    };
    Gleam_MakeType(CollisionSystem, "")
    {
        Gleam_MakeType_AddField(ccdMaxCount);
    }

    Gleam_MakeGlobalSystem(CollisionSystem)
}