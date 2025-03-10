#pragma once
#include "CustomUI.h"
#include "System/ProjectWindow.h"

namespace Gleam
{
    Gleam_AddEditorSystems(
        GlobalProjectWindow
    )

    Gleam_MakeProjectWindowMenu("Create/Scene", ProjectWindow_CreateScene)
}