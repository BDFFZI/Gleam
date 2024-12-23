#pragma once
#include "LightEngine/Runtime/ECS/System.h"
#include "LightEngine/Runtime/ECS/_Concept.hpp"
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
