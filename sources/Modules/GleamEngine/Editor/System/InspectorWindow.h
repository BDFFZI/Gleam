#pragma once
#include <variant>

#include "EditorUISystem.h"
#include "GleamEngine/Editor/EditorUI/EditorUI.h"
#include "GleamECS/Runtime/System/SystemGroup.h"

namespace Gleam
{
    struct InspectorTarget
    {
        std::weak_ptr<void> objectPtr = {};
        std::type_index objectTypeIndex = typeid(void);
        std::shared_ptr<void> ownedObject = nullptr; //由InspectorTarget托管的资源

        InspectorTarget() = default;
        InspectorTarget(nullptr_t)
            : InspectorTarget()
        {
        }
        InspectorTarget(const std::shared_ptr<void>& objectPtr, const std::type_index objectTypeIndex)
            : objectPtr(objectPtr), objectTypeIndex(objectTypeIndex)
        {
        }
        template <class T> requires !std::is_void_v<T>
        InspectorTarget(const std::shared_ptr<T>& objectPtr)
        {
            this->objectPtr = objectPtr;
            objectTypeIndex = typeid(*objectPtr.get());
        }

        /**
         * 设置一个平凡的可复制类型对象
         * @param object 
         */
        template <class T> requires
            !std::is_reference_v<T> && !std::is_pointer_v<T> && !std::is_same_v<T, InspectorTarget> && std::is_trivial_v<T>
        InspectorTarget(T object)
        {
            ownedObject = std::make_shared<T>(object);

            this->objectPtr = ownedObject;
            this->objectTypeIndex = typeid(T);
        }
        /**
         * 设置一个完全由用户负责控制生命周期的对象，用户应能确保该对象不会变成野指针！
         * @tparam T 
         * @param object 
         */
        template <class T> requires
            !std::is_reference_v<T> && !std::is_pointer_v<T> && !std::is_same_v<T, InspectorTarget> && !std::is_same_v<T, std::shared_ptr<T>>
        explicit InspectorTarget(T& object)
        {
            ownedObject = std::shared_ptr<T>(&object, [](T*)
            {
                //一个不会销毁的假共享指针
            });

            this->objectPtr = ownedObject;
            this->objectTypeIndex = typeid(T);
        }
    };

    class InspectorWindow : public System
    {
    public:
        using CustomUI = std::unordered_map<std::type_index, std::function<void(void*)>>;

        static void MakeCustomUI(std::type_index typeIndex, const std::function<void(void*)>& drawInspectorUI);
        template <class T>
        static void MakeCustomUI(const std::function<void(T&)>& drawInspectorUI)
        {
            MakeCustomUI(
                typeid(T),
                [drawInspectorUI](void* target)
                {
                    drawInspectorUI(*static_cast<T*>(target));
                }
            );
        }

        static const CustomUI& GetCustomUI();
        static bool& GetIsDebugGUI();
        static void Show(const InspectorTarget& inspectorTarget);

        InspectorWindow(): System(GlobalEditorUISystem, DefaultOrder, MaxOrder)
        {
        }

        const InspectorTarget& GetTarget() const;
        void SetTarget(const InspectorTarget& target);

    private:
        inline static CustomUI inspectorGUIs = {};
        inline static bool isDebugGUI = false;

        InspectorTarget inspectorTarget;

        void Update() override;
    };
    Gleam_MakeGlobalSystem(InspectorWindow)

#define Gleam_AddInspectorWindowUI(type,drawInspectorUI)\
    Gleam_MakeInitEvent(){InspectorWindow::MakeCustomUI(typeid(type),\
    [](void* target){drawInspectorUI(*static_cast<type##*>(target));});}
}