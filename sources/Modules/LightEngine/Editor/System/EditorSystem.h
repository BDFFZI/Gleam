#pragma once
#include "EditorUISystem.h"
#include "LightECS/Runtime/System.h"

namespace Light
{
    class EditorSystem : public System
    {
    public:
        EditorSystem(): System(std::nullopt, LeftOrder, LeftOrder)
        {

        }

        bool& IsPlaying();

    private:
        bool isPlaying = false;

        void Update() override;
    };
    Light_MakeGlobalSystem(EditorSystem)
}