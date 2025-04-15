#pragma once
#include "GleamEngine/Editor/System/EditorUISystem.h"
#include "GleamGraphics/Runtime/Resource/GRenderTarget/GRenderTexture.h"
#include "GleamUI/Runtime/UI.h"
#include "GleamWindow/Runtime/System/InputSystem.h"

namespace Gleam
{
    class GameWindow : public System<EditorUISystem, SystemMinOrder, SystemMidOrder>
    {
        friend class GameWindow_PreProcess;

        float2 imagePosition = 0;
        float2 lastImageSize = 0;
        float aspect = 1920.0f / 1080.0f;
        std::shared_ptr<GRenderTexture> renderTexture;
        ImTextureID renderTextureID = {};
        bool isDirty = false;

        void Stop() override;
        void Update() override;
    };
    Gleam_MakeEditorSystem(GameWindow)

    class GameWindow_PreProcess : public System<PostUpdateSystem>
    {
        GameWindow* gameWindow = nullptr;
        RenderingSystem* renderingSystem = nullptr;

        void Start() override;
        void Update() override;
    };
    Gleam_MakeEditorSystem(GameWindow_PreProcess)
}