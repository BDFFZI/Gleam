#pragma once
#include "LightECS/Runtime/System.h"
#include "LightEngine/Runtime/System/UISystem.h"

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
