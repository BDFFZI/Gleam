#pragma once
#include "GleamECS/Runtime/System/System.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

namespace Gleam
{
    class ScenePrefabSystem : public AbsoluteSystem<PreUpdateSystem, SystemMinOrder>
    {
        void Update() override;
    };

#ifdef GleamEngineEditor
    Gleam_MakeEditorSystem(ScenePrefabSystem);
#else
    Gleam_MakeRuntimeSystem(ScenePrefabSystem);
#endif
}