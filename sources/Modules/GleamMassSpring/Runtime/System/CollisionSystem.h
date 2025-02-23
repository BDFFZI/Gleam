#pragma once
#include "ForceSystem.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class CollisionSystem : public System
    {
    public:
        CollisionSystem(): System(ForceSystem, OrderRelation::After)
        {
        }

    private:
        Gleam_MakeType_Friend

        int ccdMaxCount = 3;

        void Update() override;
    };
    Gleam_MakeGlobalSystem(CollisionSystem)

    Gleam_MakeType(CollisionSystem_T, "")
    {
        Gleam_MakeType_AddField(ccdMaxCount);
    }
}