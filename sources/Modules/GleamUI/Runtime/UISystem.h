#pragma once

#include "GleamPresentation/Runtime/PresentationSystem.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
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
    Gleam_MakeSystemInstance(UISystem)
}