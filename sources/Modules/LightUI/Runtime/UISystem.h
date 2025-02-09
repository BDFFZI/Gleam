#pragma once

#include "LightPresentation/Runtime/PresentationSystem.h"
#include "LightECS/Runtime/System.h"

namespace Light
{
    class UISystem : public SystemGroup
    {
    public:
        UISystem(): SystemGroup(PresentationSystem, MiddleOrder, RightOrder)
        {
        }

    private:
        void Update() override;
    };
    Light_MakeSystemInstance(UISystem)
}