#pragma once
#include "GleamECS/Runtime/Entity/Archetype.h"
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"

namespace Gleam
{
    class Physics
    {
    public:
        static float3 GetGravity() { return gravity; }
        static int GetMaxCcdCount() { return maxCcdCount; }
        static void SetGravity(const float3 gravity) { Physics::gravity = gravity; }

        static Entity AddParticle(float3 position, float drag = 0.01f, float mass = 1);
        static Entity AddSpring(Entity particleA, Entity particleB, float elasticity = 0.5f);

    private:
        Gleam_MakeType_Friend

        inline static float3 gravity = {0.0f, -9.81f, 0.0f};
        inline static int maxCcdCount = 3;
    };
}