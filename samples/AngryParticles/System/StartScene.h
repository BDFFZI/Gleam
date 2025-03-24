#pragma once
#include "GleamECS/Runtime/System/System.h"
#include "GleamUI/Runtime/UISystem.h"

class StartSceneUI : public Gleam::System
{
public:
    StartSceneUI(): System(Gleam::GlobalUISystem)
    {
    }

private:
    void Update() override;
};
Gleam_MakeGlobalSystem(StartSceneUI)