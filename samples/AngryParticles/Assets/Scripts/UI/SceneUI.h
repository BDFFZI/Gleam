#pragma once
#include "GleamUI/Runtime/UISystem.h"

class StartSceneUI : public Gleam::UISubSystem
{
    void Update() override;
};
Gleam_MakeGlobalSystemWithID(StartSceneUI, "9c32129f-04c5-b850-f3c1-7ab5c4674c3f")

class MenuSceneUI : public Gleam::UISubSystem
{
    void Update() override;
};
Gleam_MakeGlobalSystemWithID(MenuSceneUI, "b62a023e-a76c-384a-6267-fe210e946aa9")

class LevelSceneUI : public Gleam::UISubSystem
{
    void Update() override;
};
Gleam_MakeGlobalSystemWithID(LevelSceneUI, "0049F432-D423-4280-8103-6E9D36060A6C")