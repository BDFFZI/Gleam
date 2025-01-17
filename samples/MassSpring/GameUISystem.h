#pragma once
#include "LightECS/Runtime/System.h"
#include "Public/UISystem.h"

class GameUISystem : public Light::System
{
public:
    GameUISystem(): System(&Light::UISystem)
    {
    }

    void Update() override;
};
inline GameUISystem GameUISystem = {};
