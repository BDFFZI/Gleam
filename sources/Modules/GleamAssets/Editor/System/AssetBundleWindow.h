#pragma once
#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamEngine/Editor/System/EditorUISystem.h"

namespace Gleam
{
    class AssetBundleWindow : public System<EditorUISystem>
    {
        void Update() override;
    };
    Gleam_MakeEditorSystem(AssetBundleWindow)
}