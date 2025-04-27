#pragma once
#include "GleamECS/Runtime/Entity/Archetype.h"
#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamMassSpring/Runtime/Physics.h"

namespace Gleam
{
    /**
     * 在用户数据默认更新顺序之前执行，用于向用户提供当前物理场景结算后的信息，以便用户根据模拟结果调整游戏数据。
     */
    class PhysicsSystem : public RelativeSystem<TimeSystem, SystemRelation::After>, public ISystemGroup
    {
        void Update() override;
    };
    Gleam_MakeTypeWithID(PhysicsSystem, "")
    {
        transferrer.TransferField("gravity", Physics::gravity);
        transferrer.TransferField("maxCcdCount", Physics::maxCcdCount);
    }
    Gleam_MakeRuntimeSystem(PhysicsSystem)
}