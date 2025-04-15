#pragma once

#include "GleamECS/Runtime/Entity/Archetype.h"
#include "GleamUI/Runtime/UI.h"
#include "GleamEngine/Editor/System/EditorUISystem.h"
#include "GleamEngine/Editor/System/InspectorWindow.h"
#include "GleamEngine/Runtime/Entity/Transform.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamRendering/Runtime/Entity/Archetype.h"
#include "GleamWindow/Runtime/System/InputSystem.h"
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    Gleam_MakeArchetypeChild(SceneCameraArchetype, CameraArchetype, ScreenToClip)

    class SceneWindow : public System<EditorUISystem>
    {
    public:
        static SceneWindow& GetSceneWindowDrawing();
        static const InspectorWindow::CustomUI& GetCustomUI();
        static void AddCustomUI(std::type_index typeIndex, const std::function<void(void*)>& drawSceneUI);

        Entity GetSceneCamera() const;
        int GetHandleOption() const;

    private:
        friend class SceneWindow_PreProcess;

        inline static std::unordered_map<std::type_index, std::function<void(void*)>> sceneGUIs = {};
        inline static SceneWindow* sceneWindowDrawing = nullptr;

        float2 windowContentPosition = 0;
        float2 windowContentSize = 0;
        //预建资源
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
        void PreUpdate();
    };
    Gleam_MakeEditorSystem(SceneWindow)

    class SceneWindow_PreProcess : public System<PostUpdateSystem>
    {
        void Update() override
        {
            SceneWindow& sceneWindow = World::GetSystemAllocator().GetSystem<SceneWindow>();
            sceneWindow.PreUpdate();
        }
    };
    Gleam_MakeEditorSystem(SceneWindow_PreProcess)

    class SceneWindowInput : public InputSystem
    {
    };
    Gleam_MakeTypeWithParent(SceneWindowInput, InputSystem)
    {
    }
    Gleam_MakeEditorSystem(SceneWindowInput)

#define Gleam_AddSceneWindowUI(type,drawSceneUI)\
    Gleam_MakeInitEvent(){SceneWindow::AddCustomUI(typeid(type),\
    [](void* target){drawSceneUI(*static_cast<type##*>(target));});}
}