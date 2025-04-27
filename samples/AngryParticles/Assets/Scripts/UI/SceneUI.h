#pragma once
#include "GleamUI/Runtime/UISystem.h"

class StartSceneUI : public Gleam::System<Gleam::UISystem>
{
    void Update() override;
};
Gleam_MakeTypeWithID(StartSceneUI, "9c32129f-04c5-b850-f3c1-7ab5c4674c3f")
{
}
Gleam_MakeSystemInfo(StartSceneUI)

class MenuSceneUI : public Gleam::System<Gleam::UISystem>
{
    void Update() override;
};
Gleam_MakeTypeWithID(MenuSceneUI, "b62a023e-a76c-384a-6267-fe210e946aa9")
{
}
Gleam_MakeSystemInfo(MenuSceneUI)

class LevelSceneUI : public Gleam::System<Gleam::UISystem>
{
    void Update() override;
};
Gleam_MakeTypeWithID(LevelSceneUI, "0049F432-D423-4280-8103-6E9D36060A6C")
{
}
Gleam_MakeSystemInfo(LevelSceneUI)