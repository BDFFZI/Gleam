#pragma once
#include "../Public/UISystem.h"
#include "LightECS/Runtime/System.h"

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
}
