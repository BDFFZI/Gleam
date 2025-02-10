#pragma once
#include <vector>

#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class Editor
    {
    public:
        static std::vector<System*>& EditorSystems();
        static bool& IsPlaying();
    private:
        static inline std::vector<System*> editorSystems;
        static inline bool isPlaying = false;
    };

#include "GleamUtility/Runtime/Program.h"
#define Gleam_AddEditorSystems(...) Gleam_MakeInitEvent(){\
        ::Gleam::Editor::EditorSystems().insert(::Gleam::Editor::EditorSystems().end(),{__VA_ARGS__});\
    }
}