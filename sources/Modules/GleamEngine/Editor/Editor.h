#pragma once
#include <vector>

#include "GleamECS/Runtime/Entity/Archetype.h"
#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamEngine/Runtime/Entity/Time.h"
#include "GleamUtility/Runtime/Macro.h"

namespace Gleam
{
    class Editor
    {
    public:
        static void AddEditorSystems(std::initializer_list<std::reference_wrapper<System>> systems);
        static void AddEditorOnlySystems(std::initializer_list<std::reference_wrapper<System>> systems);

        static bool& IsPlaying();
        static Time& GetEditorTime();

    private:
        friend void Editor_InterceptRuntimeSystem();
        friend void Editor_PlayOrStopEngine();
        static inline std::vector<SystemInfo*> editorSystems;
        static inline std::vector<SystemInfo*> editorOnlySystems;
        static inline bool isPlaying = false;
        static inline Entity editorTimeEntity;
    };

    void Editor_InterceptRuntimeSystem();
    void Editor_PlayOrStopEngine();

#define Gleam_AddEditorSystems(...) Gleam_MakeInitEvent(){\
::Gleam::Editor::AddEditorSystems({__VA_ARGS__});\
}
#define Gleam_AddEditorOnlySystems(...) Gleam_MakeInitEvent(){\
::Gleam::Editor::AddEditorOnlySystems({__VA_ARGS__});\
}
}