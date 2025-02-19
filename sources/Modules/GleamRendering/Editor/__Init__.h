#pragma once
#include "EditorUI.h"
#include "Gizmos.h"
#include "System/GameWindow.h"
#include "System/GizmosSystem.h"
#include "System/HandlesSystem.h"
#include "System/SceneWindow.h"

namespace Gleam
{
    Gleam_AddStartEvent(InitGizmosLibrary, InitGraphicsLibraryOrder+1)
    {
        Gizmos::Init();
    }

    Gleam_AddStopEvent(UnInitGizmosLibrary, UnInitGraphicsLibraryOrder-1)
    {
        Gizmos::UnInit();
    }

    Gleam_AddEditorSystems(
        GameWindow,
        SceneWindow,
        HandlesSystem::GetInstance(),
        GizmosSystem::GetInstance()
    )

    Gleam_MakeSceneGUI(Entity, SceneGUI_Entity)
    Gleam_MakeSceneGUI(LocalToWorld, SceneGUI_LocalToWorld)
    Gleam_MakeSceneGUI(Point, SceneGUI_Point)
    Gleam_MakeInspectorGUI(Point, InspectorGUI_Point)
    Gleam_MakeInspectorGUI(Segment, InspectorGUI_Segment)
    Gleam_MakeInspectorGUI(Cuboid, InspectorGUI_Cuboid)
    Gleam_MakeInspectorGUI(Sphere, InspectorGUI_Sphere)
}