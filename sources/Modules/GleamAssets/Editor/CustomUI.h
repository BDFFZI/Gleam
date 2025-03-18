#pragma once
#include "GleamAssets/Runtime/SceneAsset/EntityAsset.h"
#include "GleamECS/Runtime/Archetype.h"

namespace Gleam
{
    void ProjectWindowMenu_CreateScene();
    void ProjectWindowMenu_OpenScene();

    void InspectorWindowUI_EntityAsset(EntityAsset& entityAsset);
}