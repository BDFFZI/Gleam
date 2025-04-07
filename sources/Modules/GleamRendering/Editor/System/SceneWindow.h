#pragma once

#include "GleamECS/Runtime/Entity/Archetype.h"
#include "GleamUI/Runtime/UI.h"
#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamEngine/Editor/System/EditorUISystem.h"
#include "GleamEngine/Editor/System/InspectorWindow.h"
#include "GleamEngine/Runtime/Entity/Transform.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamRendering/Runtime/Entity/Archetype.h"
#include "GleamWindow/Runtime/System/InputSystem.h"

namespace Gleam
{
    Gleam_MakeArchetypeChild(SceneCameraArchetype, CameraArchetype, ScreenToClip)

    class SceneWindow : public System
    {
    public:
        static SceneWindow& GetSceneWindowDrawing();
        static const InspectorWindow::CustomUI& GetCustomUI();
        static void AddCustomUI(std::type_index typeIndex, const std::function<void(void*)>& drawSceneUI);


        SceneWindow(): System(GlobalEditorUISystem)
        {
        }

        Entity GetSceneCamera() const;
        InputSystem& GetSceneInputSystem();
        int GetHandleOption() const;

    private:
        inline static std::unordered_map<std::type_index, std::function<void(void*)>> sceneGUIs = {};
        inline static SceneWindow* sceneWindowDrawing = nullptr;

        float2 windowContentPosition = 0;
        float2 windowContentSize = 0;
        //预建资源
        SystemEvent preProcessSystem = SystemEvent("SceneWindow_PreProcess", GlobalPostUpdateSystem);
        InputSystem inputSystem = Create<InputSystem>("SceneWindow_Input");
        Entity sceneCamera = Entity::Null;
        //场景相机渲染目标相关
        std::unique_ptr<GRenderTexture> sceneCameraCanvas;
        ImTextureID sceneCameraCanvasImID = nullptr;
        bool isDirty = true;
        //场景UI信息
        int handleOption = 1;
        bool showSceneUI = true;
        float moveSpeed = 1.5;
        //相机位置存档（重启时使用）
        LocalTransform cameraTransformSaving = {};
        Camera cameraSaving = {};

        void Start() override;
        void Stop() override;
        void Update() override;
    };
    Gleam_MakeSystem(SceneWindow)

#define Gleam_AddSceneWindowUI(type,drawSceneUI)\
    Gleam_MakeInitEvent(){SceneWindow::AddCustomUI(typeid(type),\
    [](void* target){drawSceneUI(*static_cast<type##*>(target));});}
}