#pragma once
#include "LightEngine/Runtime/ECS/System.h"
#include "LightUI/Runtime/UISystem.h"

namespace Light
{
    class EditorUISystem : public SystemGroup
    {
    public:
        EditorUISystem(): SystemGroup(UISystem, MiddleOrder, RightOrder)
        {
        }
        void Update() override;
    };
    Light_MakeSystem(EditorUISystem)
}