#pragma once
#include <vector>

#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamUtility/Runtime/Macro.h"

namespace Gleam
{
    class Editor
    {
    public:
        static void AddEditorSystems(std::initializer_list<std::reference_wrapper<System>> systems);
        static void AddEditorOnlySystems(std::initializer_list<std::reference_wrapper<System>> systems);
        static bool& IsPlaying();

    private:
        friend void Editor_InterceptRuntimeSystem();
        friend void Editor_PlayOrStopEngine();
        static inline std::vector<std::reference_wrapper<System>> editorSystems;
        static inline std::vector<std::reference_wrapper<System>> editorOnlySystems;
        static inline bool isPlaying = false;
    };

    void Editor_InterceptRuntimeSystem();
    void Editor_PlayOrStopEngine();

#define Gleam_AddEditorSystems(...) Gleam_MakeInitEvent(){\
Editor::AddEditorSystems({__VA_ARGS__});\
}
#define Gleam_AddEditorOnlySystems(...) Gleam_MakeInitEvent(){\
Editor::AddEditorOnlySystems({__VA_ARGS__});\
}
}