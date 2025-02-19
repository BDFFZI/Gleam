#include "EditorUI.h"

#include "Gizmos.h"
#include "Handles.h"
#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Runtime/System/TransformSystem.h"
#include "System/SceneWindow.h"

namespace Gleam
{
    void DrawCuboid(const Entity entity, Cuboid& cuboid, const float4& color)
    {
        if (LocalToWorld* localToWorld; World::TryGetComponent(entity, localToWorld))
        {
            Gizmos::PushLocalToWorld(localToWorld->value);
            Gizmos::DrawWireCuboid(cuboid, color);
            Gizmos::PopLocalToWorld();
        }
        else
        {
            Gizmos::DrawWireCuboid(cuboid, color);
        }
    }
    void DrawSphere(const Entity entity, Sphere& sphere, const float4& color)
    {
        if (LocalToWorld* localToWorld; World::TryGetComponent(entity, localToWorld))
        {
            Gizmos::PushLocalToWorld(localToWorld->value);
            Gizmos::DrawWireSphere(sphere, color);
            Gizmos::PopLocalToWorld();
        }
        else
        {
            Gizmos::DrawWireSphere(sphere, color);
        }
    }

    void SceneGUI_Entity(const Entity entity)
    {
        if (World::HasEntity(entity))
            EditorUI::DrawEntityPure(entity, SceneWindow::GetCustomGUI());
    }
    void SceneGUI_LocalToWorld(LocalToWorld& localToWorld)
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
    void SceneGUI_Point(Point& point)
    {
        if (World::HasComponent<LocalToWorld>(EditorUI::GetEntityDrawing()))
            return;

        point.position = Handles::DrawHandle(point.position);
    }
    void InspectorGUI_Point(Point& point)
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
    void InspectorGUI_Segment(Segment& segment)
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

    void InspectorGUI_Cuboid(Cuboid& cuboid)
    {
        DrawCuboid(EditorUI::GetEntityDrawing(), cuboid);
    }
    void InspectorGUI_Sphere(Sphere& sphere)
    {
        DrawSphere(EditorUI::GetEntityDrawing(), sphere);
    }
}