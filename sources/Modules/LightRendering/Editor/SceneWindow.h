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
        SystemEvent preProcessSystem = SystemEvent("SceneWindow_PreProcess", PresentationSystem, OrderRelation::Before);
        class Input inputSystem = {};
        Entity sceneCamera = Entity::Null;

        std::unique_ptr<GRenderTexture> sceneCameraCanvas;
        ImTextureID sceneCameraCanvasImID = nullptr;
        float2 lastWindowSize;
        bool isDirty = false;


        void Start() override;
        void Stop() override;
        void Update() override;
    };
    Light_MakeSystem(SceneWindow)
}