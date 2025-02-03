#pragma once

#include "LightECS/Runtime/Entity.h"
#include "LightUI/Runtime/UI.h"
#include "LightECS/Runtime/System.h"
#include "LightEngine/Editor/EditorUI/EditorUISystem.h"
#include "LightWindow/Runtime/Input.h"

namespace Light
{
    class SceneWindow : public System
    {
    public:
        SceneWindow(): System(EditorUISystem)
        {
        }

    private:
        float2 windowPosition;
        float2 windowSize;
        //预建资源
        SystemEvent preProcessSystem = SystemEvent("SceneWindow_PreProcess", SimulationSystem);
        class Input inputSystem = {};
        Entity sceneCamera = Entity::Null;
        //场景相机渲染目标相关
        std::unique_ptr<GRenderTexture> sceneCameraCanvas;
        ImTextureID sceneCameraCanvasImID = nullptr;
        bool isDirty = true;
        //场景窗口UI
        int handleOption = 1;

        void Start() override;
        void Stop() override;
        void Update() override;
    };
    Light_MakeSystem(SceneWindow)
}