#pragma once
#include "LightECS/Runtime/System.h"
#include "LightUI/Runtime/UISystem.h"
#include "LightUtility/Runtime/Program.h"

namespace Light
{
    class EditorUISystem : public SystemGroup
    {
    public:
        EditorUISystem(): SystemGroup(UISystem, MiddleOrder, RightOrder)
        {
        }

        void RegisterEditorMenu(const std::string& name, const std::function<void()>& action);

    private:
        std::vector<std::tuple<std::string, std::function<void()>>> editorMenus = {};

        void Update() override;
    };
    Light_MakeSystemInstance(EditorUISystem)

#define Light_MakeEditorMenu(name,action) \
    Light_MakeInitEvent(){EditorUISystem->RegisterEditorMenu(name,action);}
}