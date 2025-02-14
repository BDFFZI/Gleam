#pragma once
#include "GameWindow.h"
#include "SceneWindow.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"
#include <ImGuizmo.h>

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
        float4x4 worldToView = SceneWindow.GetCameraWorldToLocal().value;
        float4x4 viewToClip = SceneWindow.GetCameraViewToClip().value;
        Manipulate(
            reinterpret_cast<float*>(&worldToView),
            reinterpret_cast<float*>(&viewToClip),
            imGuiOption, ImGuizmo::LOCAL,
            reinterpret_cast<float*>(&localToWorld.value)
        );
        //解析矩阵
        if (transform.has_value())
        {
            float3 position;
            float3 rotation;
            float3 scale;
            DecomposeTRS(
                localToWorld.value,
                position, rotation, scale
            );
            transform.value()->position = position;
            transform.value()->rotation = Quaternion::Euler(rotation);
            transform.value()->scale = scale;
        }
    }
    Gleam_MakeSceneGUI(LocalToWorld, SceneGUI_LocalToWorld)

    inline void SceneGUI_Entity(const Entity entity)
    {
        if (World::HasEntity(entity))
            EditorUI::DrawEntityPure(entity, SceneWindow::GetCustomGUI());
    }
    Gleam_MakeSceneGUI(Entity, SceneGUI_Entity)
}