#pragma once
#include "CustomUI.h"
#include "Gizmos.h"
#include "System/GameWindow.h"
#include "System/GizmosSystem.h"
#include "System/HandlesSystem.h"
#include "System/WorldWindow.h"

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
        GlobalGameWindow,
        GlobalWorldWindow,
        HandlesSystem::GetInstance(),
        GizmosSystem::GetInstance()
    )

    Gleam_MakeSceneUI(Entity, SceneUI_Entity)
    Gleam_MakeSceneUI(LocalToWorld, SceneUI_LocalToWorld)
    Gleam_MakeInspectorUI(Point, InspectorUI_Point)
    Gleam_MakeInspectorUI(Segment, InspectorUI_Segment)
    Gleam_MakeInspectorUI(Cuboid, InspectorUI_Cuboid)
    Gleam_MakeInspectorUI(Sphere, InspectorUI_Sphere)
}