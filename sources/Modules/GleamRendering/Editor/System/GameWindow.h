#pragma once
#include "GleamEngine/Editor/System/EditorUISystem.h"
#include "GleamGraphics/Runtime/Resource/GRenderTarget/GRenderTexture.h"
#include "GleamUI/Runtime/UI.h"

namespace Gleam
{
    class GameWindow : public System
    {
    public:
        GameWindow(): System(GlobalEditorUISystem, MinOrder, DefaultOrder)
        {
        }

    private:
        float2 imagePosition = 0;
        float2 lastImageSize = 0;
        float aspect = 1920.0f / 1080.0f;
        std::shared_ptr<GRenderTexture> renderTexture;
        ImTextureID renderTextureID = {};
        bool isDirty = false;
        SystemEvent preProcessSystem = SystemEvent("GameWindow_PreProcess", GlobalPostUpdateSystem);

        void Start() override;
        void Stop() override;
        void Update() override;
    };
    Gleam_MakeSystem(GameWindow)
}