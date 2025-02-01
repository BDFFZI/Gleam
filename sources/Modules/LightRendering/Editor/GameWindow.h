#pragma once
#include "LightEngine/Editor/EditorUI/EditorUISystem.h"
#include "LightGraphics/Runtime/Resource/GRenderTarget/GRenderTexture.h"
#include "LightUI/Runtime/UI.h"

namespace Light
{
    class GameWindow : public System
    {
    public:
        GameWindow(): System(EditorUISystem)
        {
        }

    private:
        float2 windowSize = 0;
        float2 windowPosition = 0;
        std::unique_ptr<GRenderTexture> renderTexture;
        ImTextureID renderTextureID = {};
        bool isDirty = false;
        SystemEvent systemEvent = SystemEvent("GameWindow_PreProcess", SimulationSystem);

        void Start() override;
        void Stop() override;
        void Update() override;
    };
    Light_MakeSystem(GameWindow)
}