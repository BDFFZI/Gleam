#pragma once

#include "GleamECS/Runtime/Archetype.h"
#include "GleamUI/Runtime/UI.h"
#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Editor/System/EditorUISystem.h"
#include "GleamEngine/Editor/EditorUI/EditorUI.h"
#include "GleamEngine/Runtime/Component/Transform.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamRendering/Runtime/Entity/Archetype.h"
#include "GleamWindow/Runtime/System/InputSystem.h"

namespace Gleam
{
    Gleam_MakeArchetypeChild(SceneCameraArchetype, CameraArchetype, ScreenToClip)

    class WorldWindow : public System
    {
    public:
        static WorldWindow& GetWorldWindowDrawing();
        static const CustomUI& GetCustomUI();
        static void AddCustomUI(std::type_index typeIndex, const std::function<void(void*)>& drawSceneUI);


        WorldWindow(): System(GlobalEditorUISystem)
        {
        }

        Entity GetSceneCamera() const;
        InputSystem& GetSceneInputSystem();
        TimeSystem& GetSceneTimeSystem();
        int GetHandleOption() const;

    private:
        inline static std::unordered_map<std::type_index, std::function<void(void*)>> sceneGUIs = {};
        inline static WorldWindow* sceneWindowDrawing = nullptr;

        float2 windowContentPosition = 0;
        float2 windowContentSize = 0;
        //预建资源
        SystemEvent preProcessSystem = SystemEvent("WorldWindow_PreProcess", GlobalPostUpdateSystem);
        InputSystem inputSystem = Engine::CreateSystem<InputSystem>("WorldWindow_Input");
        TimeSystem timeSystem = Engine::CreateSystem<TimeSystem>("WorldWindow_Time");
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
    Gleam_MakeGlobalSystem(WorldWindow)

#define Gleam_MakeSceneUI(type,drawSceneUI)\
    Gleam_MakeInitEvent(){WorldWindow::AddCustomUI(typeid(type),\
    [](void* target){drawSceneUI(*static_cast<type##*>(target));});}
}