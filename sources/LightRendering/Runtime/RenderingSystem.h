#pragma once
#include "LightEngine/Runtime/ECS/System.h"
#include "LightPresentation/Runtime/PresentationSystem.h"

namespace Light
{
    class RenderingSystem : public SystemGroup
    {
    public:
        RenderingSystem(): SystemGroup(PresentationSystem, LeftOrder, MiddleOrder)
        {
        }

    private:
        void Start() override;
        void Stop() override;
        void Update() override;
    };
    Light_MakeSystem(RenderingSystem)
}