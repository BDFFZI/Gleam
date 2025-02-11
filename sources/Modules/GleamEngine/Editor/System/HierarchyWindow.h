#pragma once
#include "EditorUISystem.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class HierarchyWindow : public System
    {
    public:
        HierarchyWindow(): System(UISystem)
        {
        }

    private:
        void Update() override;
    };
    Gleam_MakeGlobalSystem(HierarchyWindow)
}