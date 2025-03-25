#pragma once
#include "GleamECS/Runtime/System/System.h"
#include "GleamUI/Runtime/UISystem.h"

class MenuScene : public Gleam::System
{
public:
    MenuScene(): System(Gleam::GlobalUISystem)
    {
    }

private:
    void Update() override;
};
Gleam_MakeGlobalSystem(MenuScene)