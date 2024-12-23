#pragma once
#include "../Public/UISystem.h"
#include "LightEngine/Runtime/ECS/System.h"

namespace Light
{
    class HierarchyWindow : public System
    {
    public:
        HierarchyWindow(): System(&UISystem)
        {
        }

    private:
        void Update() override;
    };
    inline HierarchyWindow HierarchyWindow = {};
}
