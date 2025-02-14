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
        PhysicsSystem(): SystemGroup(TimeSystem, OrderRelation::After)
        {
        }

        static Entity AddMassPoint(float3 position);
        static Entity AddSpring(Entity massPointA, Entity massPointB);

        float3 GetGravity() const { return gravity; }
        float GetFixedDeltaTime() const { return fixedDeltaTime; }

    private:
        Gleam_MakeType_Friend

        float lastTime = 0;
        float fixedDeltaTime = 0.01f;
        float3 gravity = {0.0f, -9.81f, 0.0f};

        void Update() override;
    };
    Gleam_MakeGlobalSystem(PhysicsSystem)

    Gleam_MakeType(PhysicsSystem_T, "")
    {
        Gleam_MakeType_AddField(lastTime);
        Gleam_MakeType_AddField(fixedDeltaTime);
        Gleam_MakeType_AddField(gravity);
    }
}