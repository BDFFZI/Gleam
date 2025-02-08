#include "Editor.h"

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