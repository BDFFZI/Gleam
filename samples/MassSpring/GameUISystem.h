#pragma once
#include "LightECS/Runtime/System.h"
#include "LightUI/Runtime/UISystem.h"

class GameUISystem : public Light::System
{
public:
    GameUISystem(): System(Light::UISystem)
    {
    }

    void Update() override;
};
Light_MakeSystemInstance(GameUISystem)