#pragma once
#include "GameWindow.h"
#include "SceneWindow.h"
#include <ImGuizmo.h>

#include "Gizmos.h"
#include "Handles.h"

namespace Gleam
{
    Gleam_AddStartEvent(InitGizmosLibrary, InitGraphicsLibraryOrder+1)
    {
        Gizmos::Init();
    }

    Gleam_AddStopEvent(UnInitGizmosLibrary, UnInitGraphicsLibraryOrder-1)
    {
        Gizmos::UnInit();
    }

    Gleam_AddEditorSystems(
        GameWindow,
        SceneWindow,
        Handles::GetInstance(),
        GizmosSystem::GetInstance()
    )

    inline void SceneGUI_Entity(const Entity entity)
    {
        if (World::HasEntity(entity))
            EditorUI::DrawEntityPure(entity, SceneWindow::GetCustomGUI());
    }
    Gleam_MakeSceneGUI(Entity, SceneGUI_Entity)

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
            Handles::DrawHandle(imGuiOption, localToWorld.value, *transform.value());
        else
            Handles::DrawHandle(imGuiOption, localToWorld.value, std::nullopt);
    }
    Gleam_MakeSceneGUI(LocalToWorld, SceneGUI_LocalToWorld)

    inline void SceneGUI_Point(Point& point)
    {
        if (World::HasComponent<LocalToWorld>(EditorUI::GetEntityDrawing()))
            return;

        point.position = Handles::DrawHandle(point.position);
    }
    Gleam_MakeSceneGUI(Point, SceneGUI_Point)

    inline void InspectorGUI_Point(Point& point)
    {
        if (LocalToWorld* localToWorld; World::TryGetComponent(EditorUI::GetEntityDrawing(), localToWorld))
        {
            Gizmos::PushLocalToWorld(localToWorld->value);
            Gizmos::DrawPoint(point.position);
            Gizmos::PopLocalToWorld();
        }
        else
        {
            Gizmos::DrawPoint(point.position);
        }
    }
    Gleam_MakeInspectorGUI(Point, InspectorGUI_Point)

    inline void InspectorGUI_Segment(Segment& segment)
    {
        if (LocalToWorld* localToWorld; World::TryGetComponent(EditorUI::GetEntityDrawing(), localToWorld))
        {
            Gizmos::PushLocalToWorld(localToWorld->value);
            Gizmos::DrawSegment(segment);
            Gizmos::PopLocalToWorld();
        }
        else
        {
            Gizmos::DrawSegment(segment);
        }
    }
    Gleam_MakeInspectorGUI(Segment, InspectorGUI_Segment)

    inline void InspectorGUI_Cuboid(Cuboid& cuboid)
    {
        if (LocalToWorld* localToWorld; World::TryGetComponent(EditorUI::GetEntityDrawing(), localToWorld))
        {
            Gizmos::PushLocalToWorld(localToWorld->value);
            Gizmos::DrawWireCuboid(cuboid);
            Gizmos::PopLocalToWorld();
        }
        else
        {
            Gizmos::DrawWireCuboid(cuboid);
        }
    }
    Gleam_MakeInspectorGUI(Cuboid, InspectorGUI_Cuboid)

    inline void InspectorGUI_Sphere(Sphere& sphere)
    {
        if (LocalToWorld* localToWorld; World::TryGetComponent(EditorUI::GetEntityDrawing(), localToWorld))
        {
            Gizmos::PushLocalToWorld(localToWorld->value);
            Gizmos::DrawWireSphere(sphere);
            Gizmos::PopLocalToWorld();
        }
        else
        {
            Gizmos::DrawWireSphere(sphere);
        }
    }
    Gleam_MakeInspectorGUI(Sphere, InspectorGUI_Sphere)
}