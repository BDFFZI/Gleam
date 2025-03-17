#pragma once
#include <vector>

#include "GleamECS/Runtime/System.h"
#include "GleamUtility/Runtime/Macro.h"

namespace Gleam
{
    class Editor
    {
    public:
        static std::vector<std::reference_wrapper<System>>& EditorSystems();
        static std::vector<std::reference_wrapper<System>>& EditorOnlySystems();
        static bool& IsPlaying();

    private:
        static inline std::vector<std::reference_wrapper<System>> editorSystems;
        static inline std::vector<std::reference_wrapper<System>> editorOnlySystems;
        static inline bool isPlaying = false;
    };
    
    void Editor_ReplaceRuntimeSystem();
    void Editor_PlayOrStopEngine();

#define Gleam_AddEditorSystems(...) Gleam_MakeInitEvent(){\
::Gleam::Editor::EditorSystems().insert(::Gleam::Editor::EditorSystems().end(),{__VA_ARGS__});\
}
#define Gleam_AddEditorOnlySystems(...) Gleam_MakeInitEvent(){\
::Gleam::Editor::EditorOnlySystems().insert(::Gleam::Editor::EditorOnlySystems().end(),{__VA_ARGS__});\
}
}