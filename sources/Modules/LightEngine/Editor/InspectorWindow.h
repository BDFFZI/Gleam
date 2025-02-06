#pragma once
#include "EditorUISystem.h"
#include "LightECS/Runtime/Entity.h"
#include "LightECS/Runtime/System.h"

namespace Light
{
    class InspectorWindow : public System
    {
    public:
        static void RegisterInspectorGUI(std::type_index typeIndex, const std::function<void(void*)>& drawInspectorGUI);
        static void Show(Entity target);

        InspectorWindow(): System(EditorUISystem)
        {
        }

        void* GetTarget() const;
        std::type_index GetTargetType() const;
        template <class T>
        void SetTarget(T* target)
        {
            InspectorWindow::target = target;
            targetType = typeid(T);
        }

    private:
        inline static std::unordered_map<std::type_index, std::function<void(void*)>> inspectorGUIs = {};

        void* target = nullptr;
        std::type_index targetType = typeid(void*);

        void Stop() override;
        void Update() override;
    };
    Light_MakeSystem(InspectorWindow)

#define Light_MakeInspectorGUI(type,drawInspectorGUI)\
    Light_MakeInitEvent(){InspectorWindow::RegisterInspectorGUI(typeid(type),\
    [](void* target){drawInspectorGUI(static_cast<type##*>(target));});}
}