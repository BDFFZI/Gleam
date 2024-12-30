#pragma once
#include "EditorUISystem.h"
#include "LightEngine/Runtime/ECS/Entity.hpp"
#include "LightEngine/Runtime/ECS/System.h"

namespace Light
{
    class InspectorWindow : public System
    {
    public:
        Entity target = Entity::Null;

        InspectorWindow(): System(EditorUISystem)
        {
        }

    private:
        void Update() override;
    };
    Light_MakeSystem(InspectorWindow)
}
