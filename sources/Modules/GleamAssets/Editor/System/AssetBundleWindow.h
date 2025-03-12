#pragma once
#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Editor/System/EditorUISystem.h"

namespace Gleam
{
    class AssetBundleWindow : public System
    {
    public:
        AssetBundleWindow(): System(GlobalEditorUISystem)
        {
        }

    private:
        void Update() override;
    };
    Gleam_MakeGlobalSystem(AssetBundleWindow)
}