#pragma once
#include "GleamEngine/Editor/System/EditorUISystem.h"
#include "GleamGraphics/Runtime/Resource/GRenderTarget/GRenderTexture.h"
#include "GleamUI/Runtime/UI.h"

namespace Gleam
{
    class GameWindow : public System
    {
    public:
        GameWindow(): System(GlobalEditorUISystem)
        {
        }

    private:
        float2 windowPosition = 0;
        float2 windowSize = 0;
        std::unique_ptr<GRenderTexture> renderTexture;
        ImTextureID renderTextureID = {};
        bool isDirty = false;
        SystemEvent preProcessSystem = SystemEvent("GameWindow_PreProcess", GlobalPostUpdateSystem);

        void Start() override;
        void Stop() override;
        void Update() override;
    };
    Gleam_MakeGlobalSystem(GameWindow)
}