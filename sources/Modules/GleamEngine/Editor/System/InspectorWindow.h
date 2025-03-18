#pragma once
#include <variant>

#include "EditorUISystem.h"
#include "GleamEngine/Editor/EditorUI/EditorUI.h"
#include "GleamECS/Runtime/System/SystemGroup.h"

namespace Gleam
{
    struct InspectorTarget
    {
        void* data = nullptr;
        std::type_index type = typeid(void);

        InspectorTarget() = default;
        InspectorTarget(void* data, std::type_index type);
        template <class T>
            requires !std::is_same_v<T, InspectorTarget> && !std::is_reference_v<T>
        InspectorTarget(T& target)
        {
            data = &target;
            type = typeid(target);
        }
    };

    class InspectorWindow : public System
    {
    public:
        using CustomUI = std::unordered_map<std::type_index, std::function<void(void*)>>;
        
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

        std::optional<InspectorTarget> target;

        void Stop() override;
        void Update() override;
    };
    Gleam_MakeGlobalSystem(InspectorWindow)

#define Gleam_AddInspectorWindowUI(type,drawInspectorUI)\
    Gleam_MakeInitEvent(){InspectorWindow::AddCustomUI(typeid(type),\
    [](void* target){drawInspectorUI(*static_cast<type##*>(target));});}
}