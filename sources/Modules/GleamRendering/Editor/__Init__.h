#pragma once
#include "GameWindow.h"
#include "SceneWindow.h"
#include <ImGuizmo.h>

#include "Gizmos.h"

namespace Gleam
{
    Gleam_AddEditorSystems(
        GameWindow,
        SceneWindow
    )

    inline void SceneGUI_LocalToWorld(LocalToWorld& localToWorld)
    {
        //获取实体和组件
        Entity entity = EditorUI::GetEntityDrawing();
        std::optional<LocalTransform*> transform = World::TryGetComponent<LocalTransform>(entity);
        if (transform.has_value()) //LocalToWorld可能过时，显式更新一次
            TransformSystem::ComputeLocalToWorld(*transform.value(), localToWorld);
        //获取手柄类型信息
        static constexpr ImGuizmo::OPERATION options[] = {ImGuizmo::BOUNDS, ImGuizmo::TRANSLATE, ImGuizmo::ROTATE, ImGuizmo::SCALE};
        ImGuizmo::OPERATION imGuiOption = options[SceneWindow.GetHandleOption()];
        //绘制
        if (transform.has_value())
            Gizmos::DrawHandle(imGuiOption, localToWorld.value, *transform.value());
        else
            Gizmos::DrawHandle(imGuiOption, localToWorld.value, std::nullopt);
    }
    Gleam_MakeSceneGUI(LocalToWorld, SceneGUI_LocalToWorld)

    inline void SceneGUI_Entity(const Entity entity)
    {
        if (World::HasEntity(entity))
            EditorUI::DrawEntityPure(entity, SceneWindow::GetCustomGUI());
    }
    Gleam_MakeSceneGUI(Entity, SceneGUI_Entity)
}