#pragma once
#include <any>
#include <variant>

#include "EditorUISystem.h"
#include "GleamEngine/Editor/EditorUI/EditorUI.h"
#include "GleamECS/Runtime/World.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class InspectorWindow : public System
    {
    public:
        static bool& UseDebugGUI();
        static const CustomGUI& GetCustomGUI();
        static void AddCustomGUI(std::type_index typeIndex, const std::function<void(void*)>& drawInspectorGUI);
        template <class... Args>
        static void Show(Args... args)
        {
            InspectorWindow* inspectorWindow = new InspectorWindow();
            inspectorWindow->SetTarget(args...);
            World::AddSystem(*inspectorWindow);
        }

        InspectorWindow(): System(UISystem)
        {
        }

        void* GetTarget();
        std::type_index GetTargetType() const;
        template <class T> requires std::is_trivial_v<T>
        void SetTarget(T target)
        {
            this->target = target;
            targetType = typeid(T);
        }
        template <class T>
        void SetTarget(T* target)
        {
            this->target = target;
            targetType = typeid(*target);
        }
        template <class T>
        void SetTarget(const std::weak_ptr<T>& target)
        {
            this->target = target;
            targetType = typeid(*target.lock().get());
        }
        void SetTarget(const std::variant<std::any, void*, std::weak_ptr<void>>& target, std::type_index targetType);

    private:
        inline static CustomGUI inspectorGUIs = {};
        inline static bool useDebugGUI = false;

        std::variant<std::any, void*, std::weak_ptr<void>> target;
        std::type_index targetType = typeid(void);

        void Stop() override;
        void Update() override;
    };
    Gleam_MakeGlobalSystem(InspectorWindow)

#define Gleam_MakeInspectorGUI(type,drawInspectorGUI)\
    Gleam_MakeInitEvent(){InspectorWindow::AddCustomGUI(typeid(type),\
    [](void* target){drawInspectorGUI(*static_cast<type##*>(target));});}
}