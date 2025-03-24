#pragma once
#include "GleamAnimation/Runtime/Asset/AnimationClip.h"
#include "GleamAssets/Editor/System/ProjectWindow.h"
#include "System/AnimationWindow.h"

namespace Gleam
{
    Gleam_MakeCreateAssetMenu(AnimationClip, ".anim")
    Gleam_AddEditorMenu("Window/Animation", []{World::AddSystem(GlobalAnimationWindow);});
}