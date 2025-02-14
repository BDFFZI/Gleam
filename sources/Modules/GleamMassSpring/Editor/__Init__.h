#pragma once
#include "GleamRendering/Editor/Gizmos.h"

namespace Gleam
{
    inline void SceneGUI_MassPointPhysics(MassPointPhysics& massPointPhysics)
    {
        Entity entity = EditorUI::GetEntityDrawing();
        Point& point = World::GetComponent<Point>(entity);
        point.position = Gizmos::DrawHandle(point.position);
    }

    Gleam_MakeSceneGUI(MassPointPhysics, SceneGUI_MassPointPhysics)
}