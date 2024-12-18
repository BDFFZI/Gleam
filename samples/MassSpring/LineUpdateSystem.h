#pragma once
#include "LightECS/Runtime/System.h"
#include "Rendering/RenderingSystem.h"

class LineUpdateSystem : public Light::System
{
public:
    LineUpdateSystem(): System(&Light::PresentationSystem, LeftOrder)
    {
    }

    void Update() override;
};
inline LineUpdateSystem LineUpdateSystem = {};