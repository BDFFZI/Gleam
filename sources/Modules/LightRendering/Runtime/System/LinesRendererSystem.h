#pragma once
#include "RenderingSystem.h"
#include "LightECS/Runtime/System.h"

namespace Light
{
    class LinesRendererSystem : public System
    {
    public:
        LinesRendererSystem(): System(RenderingSystem, OrderRelation::Before)
        {
        }

    private:
        void Update() override;
    };
    Light_MakeSystem(LinesRendererSystem)
}