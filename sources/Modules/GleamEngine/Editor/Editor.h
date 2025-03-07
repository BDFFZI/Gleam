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
        static bool& IsPlaying();

    private:
        static inline std::vector<std::reference_wrapper<System>> editorSystems;
        static inline bool isPlaying = false;
    };

#define Gleam_AddEditorSystems(...) Gleam_MakeInitEvent(){\
        ::Gleam::Editor::EditorSystems().insert(::Gleam::Editor::EditorSystems().end(),{__VA_ARGS__});\
    }
}