#pragma once
#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamUI/Runtime/UISystem.h"
#include "GleamUtility/Runtime/Macro.h"

namespace Gleam
{
    class EditorUISystem : public AbsoluteSystemGroupT<UISystem, SystemMaxOrder>
    {
    public:
        static void MakeEditorMenu(const std::string& name, const std::function<void()>& action);

        void ShowIDStackToolWindow();
        void ShowDemoWindow();

    private:
        inline static std::vector<std::tuple<std::string, std::function<void()>>> editorMenus = {};

        TimeSystem* timeSystem = nullptr;
        bool showIDStackToolWindow = false;
        bool showDemoWindow = false;
        
        void Start() override;
        void Update() override;
    };
    Gleam_MakeSystem(EditorUISystem)

#define Gleam_AddEditorMenu(name,action) \
    Gleam_MakeInitEvent(){EditorUISystem::MakeEditorMenu(name,action);}
}