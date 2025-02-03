#pragma once
#include "EditorUI/EditorUISystem.h"
#include "LightECS/Runtime/Entity.h"
#include "LightECS/Runtime/System.h"

namespace Light
{
    class InspectorWindow : public System
    {
    public:
        static void Show(Entity target);

        Entity target = Entity::Null;

        InspectorWindow(): System(EditorUISystem)
        {
        }

    private:
        void Stop() override;
        void Update() override;
    };
    Light_MakeSystem(InspectorWindow)
}