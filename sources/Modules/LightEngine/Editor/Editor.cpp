#include "Editor.h"

#include "LightECS/Runtime/World.h"

namespace Light
{
    std::vector<System*>& Editor::EditorSystems()
    {
        return editorSystems;
    }
    bool& Editor::IsPlaying()
    {
        return isPlaying;
    }
}