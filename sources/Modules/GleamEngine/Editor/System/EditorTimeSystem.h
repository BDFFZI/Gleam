#pragma once
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamEngine/Editor/Editor.h"

namespace Gleam
{
    class EditorTimeSystem : public TimeSystem
    {
    };
    Gleam_MakeTypeWithParent(EditorTimeSystem, TimeSystem)
    {
    }
    Gleam_MakeEditorSystem(EditorTimeSystem)
}