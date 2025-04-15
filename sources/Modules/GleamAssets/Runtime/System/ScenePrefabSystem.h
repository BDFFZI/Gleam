#pragma once
#include "GleamECS/Runtime/System/System.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"

namespace Gleam
{
    class ScenePrefabSystem : public AbsoluteSystem<PreUpdateSystem, SystemMinOrder>
    {
        void Update() override;
    };
    Gleam_MakeRuntimeSystem(ScenePrefabSystem);
}