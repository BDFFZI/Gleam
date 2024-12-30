#pragma once
#include "EditorUISystem.h"
#include "LightEngine/Runtime/ECS/System.h"

namespace Light
{
    class HierarchyWindow : public System
    {
    public:
        HierarchyWindow(): System(EditorUISystem)
        {
        }

    private:
        void Update() override;
    };
    Light_MakeSystem(HierarchyWindow)
}