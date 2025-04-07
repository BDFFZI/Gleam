#pragma once
#include "GleamECS/Runtime/System/System.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"

namespace Gleam
{
    class ScenePrefabSystem : public System
    {
    public:
        ScenePrefabSystem(): System(GlobalPreUpdateSystem, MinOrder, MinOrder)
        {
        }

    private:
        void Update() override;
    };
    Gleam_MakeSystem(ScenePrefabSystem);
}