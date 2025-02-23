#pragma once
#include "GleamECS/Runtime/System.h"
#include "GleamUI/Runtime/UISystem.h"

class GameUISystem : public Gleam::System
{
public:
    GameUISystem(): System(Gleam::UISystem)
    {
    }

    void Update() override;
};
Gleam_MakeGlobalSystem(GameUISystem)