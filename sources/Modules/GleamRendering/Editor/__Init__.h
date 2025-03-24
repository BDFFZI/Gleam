#pragma once
#include "CustomUI.h"
#include "Gizmos.h"
#include "System/GameWindow.h"
#include "System/GizmosSystem.h"
#include "System/SceneWindow.h"

#include "GleamPresentation/Runtime/__Init__.h"
#include "GleamEngine/Editor/Editor.h"

namespace Gleam
{
    Gleam_MakeEngineEvent(Start, InitGizmosLibrary, InitGraphicsLibraryOrder+1)
    {
        Gizmos::Init();
    }
    Gleam_MakeEngineEvent(Stop, UnInitGizmosLibrary, UnInitGraphicsLibraryOrder-1)
    {
        Gizmos::UnInit();
    }

    Gleam_AddEditorSystems(
        GlobalGameWindow,
        GlobalSceneWindow,
        GlobalGizmosSystem
    )

    Gleam_AddInspectorWindowUI(Point, InspectorWindowUI_Point)
    Gleam_AddInspectorWindowUI(Segment, InspectorWindowUI_Segment)
    Gleam_AddInspectorWindowUI(Cuboid, InspectorWindowUI_Cuboid)
    Gleam_AddInspectorWindowUI(Sphere, InspectorWindowUI_Sphere)

    Gleam_AddSceneWindowUI(Entity, SceneWindowUI_Entity)
    Gleam_AddSceneWindowUI(LocalToWorld, SceneWindowUI_LocalToWorld)
}