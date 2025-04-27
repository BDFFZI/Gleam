#pragma once
#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamUI/Runtime/UISystem.h"

class GameUISystem : public Gleam::System<Gleam::UISystem>
{
    void Update() override;
};
Gleam_MakeRuntimeSystem(GameUISystem)