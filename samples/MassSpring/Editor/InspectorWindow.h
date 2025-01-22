#pragma once
#include "LightECS/Runtime/System.h"
#include "LightECS/Runtime/_Concept.h"
#include "../Public/UISystem.h"

namespace Light
{
    class InspectorWindow : public System
    {
    public:
        Entity target = Entity::Null;

        InspectorWindow(): System(&UISystem)
        {
        }

    private:
        void Update() override;
    };
    inline InspectorWindow InspectorWindow;
}
