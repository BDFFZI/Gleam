#pragma once
#include "EditorUISystem.h"
#include "EditorUIUtility.h"
#include "LightEngine/Runtime/ECS/System.h"

namespace Light
{
    class ProjectWindow : public System
    {
    public:
        ProjectWindow(): System(EditorUISystem)
        {
        }
    private:
        void Update() override;
    };
    Light_MakeSystem(ProjectWindow)
}