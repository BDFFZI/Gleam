#include "CustomUI.h"
#include "GleamEngine/Editor/CustomUI.h"

#include "Gizmos.h"
#include "Handles.h"
#include "GleamECS/Runtime/World/World.h"
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

    void InspectorWindowUI_Point(Point& point)
    {
        EditorUI::DrawSerializedContent(&point, typeid(point));
        if (LocalToWorld* localToWorld; World::TryGetComponent(InspectorWindowUI_Entity_Target, localToWorld))
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
    void InspectorWindowUI_Segment(Segment& segment)
    {
        EditorUI::DrawSerializedContent(&segment, typeid(segment));
        if (LocalToWorld* localToWorld; World::TryGetComponent(InspectorWindowUI_Entity_Target, localToWorld))
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
    void InspectorWindowUI_Cuboid(Cuboid& cuboid)
    {
        EditorUI::DrawSerializedContent(&cuboid, typeid(cuboid));
        DrawCuboid(InspectorWindowUI_Entity_Target, cuboid);
    }
    void InspectorWindowUI_Rectangle(Rectangle& rectangle)
    {
        EditorUI::DrawSerializedContent(&rectangle, typeid(rectangle));
        DrawRectangle(InspectorWindowUI_Entity_Target, rectangle);
    }
    void InspectorWindowUI_Sphere(Sphere& sphere)
    {
        EditorUI::DrawSerializedContent(&sphere, typeid(sphere));
        DrawSphere(InspectorWindowUI_Entity_Target, sphere);
    }

    void SceneWindowUI_Entity(const Entity entity)
    {
        if (!World::HasEntity(entity))
            return;

        SceneWindowUI_Entity_Target = entity;
        EntityInfo entityInfo = World::GetEntityInfoAllocator().GetEntityInfo(entity);
        const Archetype& archetype = *entityInfo.archetype;

        for (int i = 0; i < archetype.GetComponentCount(); ++i)
        {
            const Type& componentType = archetype.GetComponentType(i);
            void* component = entityInfo.memoryAddress + archetype.GetComponentOffset(i);
            std::type_index componentTypeIndex = componentType.GetIndex();

            //绘制组件内容
            ImGui::PushID(component);
            if (SceneWindow::GetCustomUI().contains(componentTypeIndex))
                SceneWindow::GetCustomUI().at(componentTypeIndex)(component);
            ImGui::PopID();
        }
    }
    void SceneWindowUI_LocalToWorld(LocalToWorld& localToWorld)
    {
        //获取实体和组件
        Entity entity = SceneWindowUI_Entity_Target;
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
}