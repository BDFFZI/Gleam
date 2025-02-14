#pragma once

#include "GleamECS/Runtime/Entity.h"
#include "GleamUI/Runtime/UI.h"
#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Editor/System/EditorUISystem.h"
#include "GleamEngine/Editor/EditorUI/EditorUI.h"
#include "GleamEngine/Runtime/Component/Transform.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamWindow/Runtime/System/InputSystem.h"

namespace Gleam
{
    class SceneWindow : public System
    {
    public:
        static SceneWindow& GetSceneWindowDrawing();
        static const CustomGUI& GetCustomGUI();
        static void AddCustomGUI(std::type_index typeIndex, const std::function<void(void*)>& drawSceneGUI);


        SceneWindow(): System(EditorUISystem)
        {
        }

        int GetHandleOption() const;

    private:
        inline static std::unordered_map<std::type_index, std::function<void(void*)>> sceneGUIs = {};
        inline static SceneWindow* sceneWindowDrawing = nullptr;

        float2 windowPosition = 0;
        float2 windowSize = 0;
        //预建资源
        SystemEvent preProcessSystem = SystemEvent("SceneWindow_PreProcess", PostUpdateSystem);
        class InputSystem inputSystem = Engine::CreateSystem<InputSystem_T>("SceneWindow_Input");
        class TimeSystem timeSystem = Engine::CreateSystem<TimeSystem_T>("SceneWindow_Time");
        Entity sceneCamera = Entity::Null;
        //场景相机渲染目标相关
        std::unique_ptr<GRenderTexture> sceneCameraCanvas;
        ImTextureID sceneCameraCanvasImID = nullptr;
        bool isDirty = true;
        //场景UI信息
        int handleOption = 1;
        //相机位置存档（重启时使用）
        LocalTransform cameraLocalTransform = {};

        void Start() override;
        void Stop() override;
        void Update() override;
    };
    Gleam_MakeGlobalSystem(SceneWindow)

#define Gleam_MakeSceneGUI(type,drawSceneGUI)\
    Gleam_MakeInitEvent(){SceneWindow::AddCustomGUI(typeid(type),\
    [](void* target){drawSceneGUI(*static_cast<type##*>(target));});}
}