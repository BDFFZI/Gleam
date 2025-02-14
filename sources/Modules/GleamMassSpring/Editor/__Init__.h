#pragma once
#include "GleamMassSpring/Runtime/Component/MassPointPhysics.h"
#include "GleamRendering/Editor/SceneWindow.h"
#include "ImGuizmo.h"

namespace Gleam
{
    inline void SceneGUI_MassPointPhysics(MassPointPhysics& massPointPhysics)
    {
        SceneWindow_T& sceneWindow = SceneWindow_T::GetSceneWindowDrawing();
        sceneWindow.DrawHandle()
    }

    Gleam_MakeSceneGUI(MassPointPhysics,)
}