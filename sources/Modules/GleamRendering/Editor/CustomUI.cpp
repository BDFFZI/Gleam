#include "CustomUI.h"

#include "Gizmos.h"
#include "Handles.h"
#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Runtime/System/TransformSystem.h"
#include "System/SceneWindow.h"

namespace Gleam
{
    void DrawRectangle(const Entity entity, const Rectangle& rectangle, const float4& color)
    {
        if (LocalToWorld* localToWorld; World::TryGetComponent(entity, localToWorld))
        {
            Gizmos::PushLocalToWorld(localToWorld->value);
            Gizmos::DrawWire(rectangle, color);
            Gizmos::PopLocalToWorld();
        }
        else
        {
            Gizmos::DrawWire(rectangle, color);
        }
    }
    void DrawCuboid(const Entity entity, const Cuboid& cuboid, const float4& color)
    {
        if (LocalToWorld* localToWorld; World::TryGetComponent(entity, localToWorld))
        {
            Gizmos::PushLocalToWorld(localToWorld->value);
            Gizmos::DrawWire(cuboid, color);
            Gizmos::PopLocalToWorld();
        }
        else
        {
            Gizmos::DrawWire(cuboid, color);
        }
    }
    void DrawSphere(const Entity entity, const Sphere& sphere, const float4& color)
    {
        if (LocalToWorld* localToWorld; World::TryGetComponent(entity, localToWorld))
        {
            Gizmos::PushLocalToWorld(localToWorld->value);
            Gizmos::DrawWire(sphere, color);
            Gizmos::PopLocalToWorld();
        }
        else
        {
            Gizmos::DrawWire(sphere, color);
        }
    }

    void SceneUI_Entity(const Entity entity)
    {
        if (World::HasEntity(entity))
            EditorUI::DrawEntityPure(entity, SceneWindow::GetCustomUI());
    }
    void SceneUI_LocalToWorld(LocalToWorld& localToWorld)
    {
        //获取实体和组件
        Entity entity = EditorUI::GetEntityDrawing();
        std::optional<LocalTransform*> transform = World::TryGetComponent<LocalTransform>(entity);
        if (transform.has_value()) //LocalToWorld可能过时，显式更新一次
            TransformSystem::ComputeLocalToWorld(*transform.value(), localToWorld);
        //获取手柄类型信息
        static constexpr ImGuizmo::OPERATION options[] = {ImGuizmo::BOUNDS, ImGuizmo::TRANSLATE, ImGuizmo::ROTATE, ImGuizmo::SCALE};
        ImGuizmo::OPERATION imGuiOption = options[GlobalSceneWindow.GetHandleOption()];
        //绘制
        if (transform.has_value())
            Handles::DrawHandle(imGuiOption, localToWorld.value, *transform.value());
        else
            Handles::DrawHandle(imGuiOption, localToWorld.value, std::nullopt);
    }

    void InspectorUI_Point(Point& point)
    {
        EditorUI::DrawDefaultInspectorUI(&point, typeid(point));
        if (LocalToWorld* localToWorld; World::TryGetComponent(EditorUI::GetEntityDrawing(), localToWorld))
        {
            Gizmos::PushLocalToWorld(localToWorld->value);
            Gizmos::Draw(point.position);
            Gizmos::PopLocalToWorld();
        }
        else
        {
            Gizmos::Draw(point.position);
        }
    }
    void InspectorUI_Segment(Segment& segment)
    {
        EditorUI::DrawDefaultInspectorUI(&segment, typeid(segment));
        if (LocalToWorld* localToWorld; World::TryGetComponent(EditorUI::GetEntityDrawing(), localToWorld))
        {
            Gizmos::PushLocalToWorld(localToWorld->value);
            Gizmos::Draw(segment);
            Gizmos::PopLocalToWorld();
        }
        else
        {
            Gizmos::Draw(segment);
        }
    }
    void InspectorUI_Cuboid(Cuboid& cuboid)
    {
        EditorUI::DrawDefaultInspectorUI(&cuboid, typeid(cuboid));
        DrawCuboid(EditorUI::GetEntityDrawing(), cuboid);
    }
    void InspectorUI_Rectangle(Rectangle& rectangle)
    {
        EditorUI::DrawDefaultInspectorUI(&rectangle, typeid(rectangle));
        DrawRectangle(EditorUI::GetEntityDrawing(), rectangle);
    }
    void InspectorUI_Sphere(Sphere& sphere)
    {
        EditorUI::DrawDefaultInspectorUI(&sphere, typeid(sphere));
        DrawSphere(EditorUI::GetEntityDrawing(), sphere);
    }
}