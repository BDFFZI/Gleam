#pragma once
#include "PositionSystem.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class ForceSystem : public System
    {
    public:
        ForceSystem(): System(GlobalPositionSystem, OrderRelation::After)
        {
        }

        float3 GetGravity() const { return gravity; }
        void SetGravity(const float3 gravity) { this->gravity = gravity; }

    private:
        Gleam_MakeType_Friend

        float3 gravity = {0.0f, -9.81f, 0.0f};

        void Update() override;
    };
    Gleam_MakeType(ForceSystem, "")
    {
        Gleam_MakeType_AddField(gravity);
    }

    Gleam_MakeGlobalSystem(ForceSystem);
}