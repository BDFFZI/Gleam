#pragma once
#include "GleamAssets/Runtime/Asset/EntityAsset.h"

namespace Gleam
{
    void ProjectWindowMenu_CreateScene();
    void ProjectWindowMenu_OpenScene();
    void ProjectWindowEvent_RenameScene(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);

    void InspectorWindowUI_EntityAsset(EntityAsset& entityAsset);
}