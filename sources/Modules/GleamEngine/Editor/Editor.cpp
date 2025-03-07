﻿#include "Editor.h"

#include "GleamECS/Runtime/World.h"

namespace Gleam
{
    std::vector<std::reference_wrapper<System>>& Editor::EditorSystems()
    {
        return editorSystems;
    }
    bool& Editor::IsPlaying()
    {
        return isPlaying;
    }
}