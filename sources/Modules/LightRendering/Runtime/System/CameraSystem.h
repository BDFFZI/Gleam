#pragma once
#include "RenderingSystem.h"
#include "LightECS/Runtime/System.h"

namespace Light
{
    class CameraSystem : public System
    {
    public:
        CameraSystem(): System(RenderingSystem, OrderRelation::Before)
        {
        }

    private:
        void Update() override;
    };
    Light_MakeSystem(CameraSystem)
}