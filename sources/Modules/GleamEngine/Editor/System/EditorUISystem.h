#pragma once
#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamUI/Runtime/UISystem.h"
#include "GleamUtility/Runtime/Macro.h"

namespace Gleam
{
    class EditorUISystem : public SystemGroup
    {
    public:
        static void MakeEditorMenu(const std::string& name, const std::function<void()>& action);
        static void ShowIDStackToolWindow();
        static void ShowDemoWindow();

    private:
        Gleam_MakeType_Friend

        inline static std::vector<std::tuple<std::string, std::function<void()>>> editorMenus = {};
        inline static bool showIDStackToolWindow = false;
        inline static bool showDemoWindow = false;

        void Start() override;
        void Update() override;

        EditorUISystem(): SystemGroup(GlobalUISystem, MaxOrder, MaxOrder)
        {
        }
    };
    Gleam_MakeSystem(EditorUISystem)

#define Gleam_AddEditorMenu(name,action) \
    Gleam_MakeInitEvent(){EditorUISystem::MakeEditorMenu(name,action);}
}