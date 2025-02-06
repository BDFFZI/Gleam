#pragma once

#include "LightRendering/Runtime/__Init__.h"
#include "LightEngine/Editor/__Init__.h"

#include "GameWindow.h"
#include "SceneWindow.h"
#include "LightMath/Runtime/LinearAlgebra/MatrixMath.h"
#include <ImGuizmo.h>

namespace Light
{
    Light_AddSystems(
        GameWindow,
        SceneWindow
    )

    inline void SceneGUI_Entity(Entity* entity)
    {
        Entity target = *entity;
        if (World::HasEntity(target) && World::HasComponent<LocalToWorld>(target))
        {
            //获取组件
            LocalToWorld& localToWorld = World::GetComponent<LocalToWorld>(target);
            std::optional<LocalTransform*> transform = World::TryGetComponent<LocalTransform>(target);
            if (transform.has_value()) //LocalToWorld可能过时，显式更新一次
                TransformSystem::ComputeLocalToWorld(*transform.value(), localToWorld);
            //获取手柄类型信息
            static constexpr ImGuizmo::OPERATION options[] = {ImGuizmo::BOUNDS, ImGuizmo::TRANSLATE, ImGuizmo::ROTATE, ImGuizmo::SCALE};
            ImGuizmo::OPERATION imGuiOption = options[SceneWindow->GetHandleOption()];
            //绘制
            float4x4 worldToView = SceneWindow->GetCameraTransform().worldToView;
            float4x4 viewToClip = SceneWindow->GetCameraTransform().viewToClip;
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
    }
    Light_MakeSceneGUI(Entity, SceneGUI_Entity)
}