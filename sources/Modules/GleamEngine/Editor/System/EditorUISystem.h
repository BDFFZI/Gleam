#pragma once
#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamUI/Runtime/UISystem.h"
#include "GleamUtility/Runtime/Macro.h"

namespace Gleam
{
    class EditorUISystem : public SystemGroup
    {
    public:
        EditorUISystem(): SystemGroup(GlobalUISystem, MaxOrder, MaxOrder)
        {
        }

        void AddEditorMenu(const std::string& name, const std::function<void()>& action);

    private:
        std::vector<std::tuple<std::string, std::function<void()>>> editorMenus = {};

        void Start() override;
        void Update() override;
    };
    Gleam_MakeGlobalSystem(EditorUISystem)

#define Gleam_AddEditorMenu(name,action) \
    Gleam_MakeInitEvent(){GlobalEditorUISystem.AddEditorMenu(name,action);}
}