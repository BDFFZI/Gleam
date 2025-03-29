#pragma once
#include "GleamECS/Runtime/System/System.h"
#include "GleamUI/Runtime/UISystem.h"

class MenuSceneUI : public Gleam::System
{
public:
    MenuSceneUI(): System(Gleam::GlobalUISystem)
    {
    }

private:
    void Update() override;
};
Gleam_MakeGlobalSystem(MenuSceneUI)