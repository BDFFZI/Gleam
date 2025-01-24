#pragma once
#include "RenderingSystem.h"
#include "LightECS/Runtime/System.h"

namespace Light
{
    class PointsRendererSystem : public System
    {
    public:
        PointsRendererSystem(): System(RenderingSystem, OrderRelation::Before)
        {
        }

    private:
        void Update() override;
    };
    Light_MakeSystem(PointsRendererSystem)
}