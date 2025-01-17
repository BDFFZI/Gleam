#pragma once
#include "EditorUISystem.h"
#include "LightECS/Runtime/Entity.hpp"
#include "LightECS/Runtime/System.h"

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
