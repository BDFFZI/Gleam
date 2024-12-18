#pragma once
#include "PresentationSystem.h"

namespace Light
{
    class UISystem : public SystemGroup
    {
    public:
        UISystem(): SystemGroup(&PresentationSystem, MiddleOrder, RightOrder)
        {
        }

        void Update() override;
    };
    inline UISystem UISystem = {};
}
