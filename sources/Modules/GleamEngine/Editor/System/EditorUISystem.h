#pragma once
#include "GleamECS/Runtime/System.h"
#include "GleamUI/Runtime/UISystem.h"
#include "GleamUtility/Runtime/Macro.h"

namespace Gleam
{
    class EditorUISystem : public SystemGroup
    {
    public:
        EditorUISystem(): SystemGroup(UISystem, MaxOrder, MaxOrder)
        {
        }

        void RegisterEditorMenu(const std::string& name, const std::function<void()>& action);

    private:
        std::vector<std::tuple<std::string, std::function<void()>>> editorMenus = {};

        void Update() override;
    };
    Gleam_MakeGlobalSystem(EditorUISystem)

#define Gleam_MakeEditorMenu(name,action) \
    Gleam_MakeInitEvent(){EditorUISystem.RegisterEditorMenu(name,action);}
}