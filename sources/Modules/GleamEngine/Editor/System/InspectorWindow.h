#pragma once
#include <variant>

#include "EditorUISystem.h"
#include "GleamEngine/Editor/EditorUI/EditorUI.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    struct InspectorTarget
    {
        void* data = nullptr;
        std::type_index type = typeid(void);

        InspectorTarget() = default;
        template <class T> requires !std::is_same_v<T, InspectorTarget>
        InspectorTarget(T& target)
        {
            data = &target;
            type = typeid(target);
        }
    };

    class InspectorWindow : public System
    {
    public:
        static Entity GetEntityDrawing();
        static void DrawDefaultContent(void* target, std::type_index targetType);
        static void DrawEntityContent(Entity entity, const CustomUI& componentGUI = {}, bool pure = false);

        static bool& UseDebugGUI();
        static const CustomUI& GetCustomUI();
        static void AddCustomUI(std::type_index typeIndex, const std::function<void(void*)>& drawInspectorUI);
        static void Show(InspectorTarget inspectorTarget);

        InspectorWindow(): System(GlobalEditorUISystem, DefaultOrder, MaxOrder)
        {
        }

        const std::optional<InspectorTarget>& GetTarget() const;
        void SetTarget(const std::optional<InspectorTarget>& target);

    private:
        inline static CustomUI inspectorGUIs = {};
        inline static bool useDebugGUI = false;
        inline static Entity drawing = Entity::Null;

        std::optional<InspectorTarget> target;

        void Stop() override;
        void Update() override;
    };
    Gleam_MakeGlobalSystem(InspectorWindow)

#define Gleam_MakeInspectorUI(type,drawInspectorUI)\
    Gleam_MakeInitEvent(){InspectorWindow::AddCustomUI(typeid(type),\
    [](void* target){drawInspectorUI(*static_cast<type##*>(target));});}
}