#pragma once
#include "LightECS/Runtime/System.h"
#include "LightEngine/Runtime/System/TimeSystem.h"
#include "LightMath/Runtime/LinearAlgebra/Vector.h"

namespace Light
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

        float3 GetGravity() const { return gravity; }
        float GetFixedDeltaTime() const { return fixedDeltaTime; }

    private:
        float lastTime = 0;
        float fixedDeltaTime = 0.01f;
        float3 gravity = {0.0f, -9.81f, 0.0f};

        void Update() override;
    };
    Light_MakeGlobalSystem(PhysicsSystem)
}