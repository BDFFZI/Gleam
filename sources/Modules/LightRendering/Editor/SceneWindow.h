#pragma once

#include "LightECS/Runtime/Entity.h"
#include "LightUI/Runtime/UI.h"
#include "LightECS/Runtime/System.h"
#include "LightEngine/Editor/System/EditorUISystem.h"
#include "LightEngine/Editor/EditorUI/EditorUI.h"
#include "LightRendering/Runtime/Component/Camera.h"
#include "LightWindow/Runtime/Input.h"

namespace Light
{
    class SceneWindow : public System
    {
    public:
        static const CustomGUI& GetCustomGUI();
        static void AddCustomGUI(std::type_index typeIndex, const std::function<void(void*)>& drawSceneGUI);

        const Camera& GetCamera() const;
        const WorldToClip& GetCameraTransform() const;
        int GetHandleOption() const;

        SceneWindow(): System(EditorUISystem)
        {
        }

    private:
        inline static std::unordered_map<std::type_index, std::function<void(void*)>> sceneGUIs = {};

        float2 windowPosition = 0;
        float2 windowSize = 0;
        //预建资源
        SystemEvent preProcessSystem = SystemEvent("SceneWindow_PreProcess", PostUpdateSystem);
        class Input inputSystem = {"SceneWindow_Input"};
        Entity sceneCamera = Entity::Null;
        //场景相机渲染目标相关
        std::unique_ptr<GRenderTexture> sceneCameraCanvas;
        ImTextureID sceneCameraCanvasImID = nullptr;
        bool isDirty = true;
        //场景UI信息
        Camera camera = {};
        WorldToClip cameraTransform = {};
        int handleOption = 1;

        void Start() override;
        void Stop() override;
        void Update() override;
    };
    Light_MakeGlobalSystem(SceneWindow)

#define Light_MakeSceneGUI(type,drawSceneGUI)\
    Light_MakeInitEvent(){SceneWindow::AddCustomGUI(typeid(type),\
    [](void* target){drawSceneGUI(*static_cast<type##*>(target));});}
}