#pragma once

#include "GleamPresentation/Runtime/PresentationSystem.h"
#include "GleamECS/Runtime/System.h"

#ifdef Gleam_Rendering
#include "GleamRendering/Runtime/System/RenderingSystem.h"
#define Order RenderingSystem,OrderRelation::After
#else
#define Order PresentationSystem
#endif

namespace Gleam
{
    class UISystem : public SystemGroup
    {
    public:
        UISystem(): SystemGroup(Order)
        {
        }

    private:
        void Update() override;
    };
    Gleam_MakeGlobalSystem(UISystem)
}

#undef Order