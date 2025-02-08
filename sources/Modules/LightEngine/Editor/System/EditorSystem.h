#pragma once
#include "EditorUISystem.h"
#include "LightECS/Runtime/System.h"

namespace Light
{
    class EditorSystem : public System
    {
    public:
        EditorSystem(): System(std::nullopt, RightOrder, RightOrder)
        {
        }

    private:
        bool lastIsPlaying = false;

        void Update() override;
    };
    Light_MakeSystemInstance(EditorSystem)
}