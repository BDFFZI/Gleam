#pragma once
#include <any>
#include <variant>

#include "EditorUISystem.h"
#include "GleamEngine/Editor/EditorUI/EditorUI.h"
#include "GleamECS/Runtime/World.h"
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
            type = typeid(T);
        }
    };

    class InspectorWindow : public System
    {
    public:
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

#define Gleam_MakeInspectorUI(type,drawInspectorUI)\
    Gleam_MakeInitEvent(){InspectorWindow::AddCustomUI(typeid(type),\
    [](void* target){drawInspectorUI(*static_cast<type##*>(target));});}
}