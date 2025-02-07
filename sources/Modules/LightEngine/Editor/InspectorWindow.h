#pragma once
#include "EditorUISystem.h"
#include "EditorUI/EditorUI.h"
#include "LightECS/Runtime/Entity.h"
#include "LightECS/Runtime/System.h"

namespace Light
{
    class InspectorWindow : public System
    {
    public:
        static bool& UseDebugGUI();
        static const CustomGUI& GetCustomGUI();
        static void AddCustomGUI(std::type_index typeIndex, const std::function<void(void*)>& drawInspectorGUI);
        static void Show(Entity target);
        static void Show(void* target, std::type_index targetType );

        InspectorWindow(): System(EditorUISystem)
        {
        }

        void* GetTarget() const;
        std::type_index GetTargetType() const;

        template <class T>
        void SetTarget(T* target)
        {
            InspectorWindow::target = target;
            targetType = typeid(*target);
        }

    private:
        inline static CustomGUI inspectorGUIs = {};
        inline static bool useDebugGUI = false;

        void* target = nullptr;
        std::type_index targetType = typeid(void*);

        void Stop() override;
        void Update() override;
    };
    Light_MakeSystem(InspectorWindow)

#define Light_MakeInspectorGUI(type,drawInspectorGUI)\
    Light_MakeInitEvent(){InspectorWindow::AddCustomGUI(typeid(type),\
    [](void* target){drawInspectorGUI(static_cast<type##*>(target));});}
}