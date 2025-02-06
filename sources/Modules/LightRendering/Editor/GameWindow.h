#pragma once
#include "LightEngine/Editor/EditorUISystem.h"
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
        float2 windowPosition = 0;
        float2 windowSize = 0;
        std::unique_ptr<GRenderTexture> renderTexture;
        ImTextureID renderTextureID = {};
        bool isDirty = false;
        SystemEvent systemEvent = SystemEvent("GameWindow_PreProcess", PostUpdateSystem);

        void Start() override;
        void Stop() override;
        void Update() override;
    };
    Light_MakeSystem(GameWindow)
}