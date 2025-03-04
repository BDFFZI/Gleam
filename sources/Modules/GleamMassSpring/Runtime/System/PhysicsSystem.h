#pragma once
#include "GleamECS/Runtime/Entity.h"
#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"

namespace Gleam
{
    /**
     * 在用户数据默认更新顺序之前执行，用于向用户提供当前物理场景结算后的信息，以便用户根据模拟结果调整游戏数据。
     */
    class PhysicsSystem : public SystemGroup
    {
    public:
        PhysicsSystem(): SystemGroup(GlobalTimeSystem, OrderRelation::After)
        {
        }

        static Entity AddParticle(float3 position, float drag = 0.01f, float mass = 1);
        static Entity AddSpring(Entity particleA, Entity particleB, float elasticity = 0.5f);

    private:
        void Update() override;
    };
    Gleam_MakeGlobalSystem(PhysicsSystem)
}