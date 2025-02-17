#pragma once
#include "GleamRendering/Editor/Handles.h"
#include "System/PointSelector.h"

namespace Gleam
{
    Gleam_AddEditorSystems(PointSelector)

    inline void SceneGUI_Point(Point&)
    {
        Entity entity = EditorUI::GetEntityDrawing();
        Point& point = World::GetComponent<Point>(entity);
        point.position = Handles::DrawHandle(point.position);
    }

    Gleam_MakeSceneGUI(Point, SceneGUI_Point)
}