#pragma once
#include <vector>

#include "LightECS/Runtime/System.h"

namespace Light
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

#include "LightUtility/Runtime/Program.h"
#define Light_AddEditorSystems(...) Light_MakeInitEvent(){\
        ::Light::Editor::EditorSystems().insert(::Light::Editor::EditorSystems().end(),{__VA_ARGS__});\
    }
}