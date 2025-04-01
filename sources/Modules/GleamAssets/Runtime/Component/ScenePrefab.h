#pragma once
#include "GleamAssets/Runtime/Asset/EntityAsset.h"
#include "GleamAssets/Runtime/Asset/SceneAsset.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/EditorUI/EditorUISerializer.h"
#endif

namespace Gleam
{
    struct ScenePrefab
    {
        std::weak_ptr<SceneAsset> sourceScene;
        Scene* instanceScene;
    };
    Gleam_MakeTypeWithID(ScenePrefab, "08A2F957-EB6B-47F7-9071-B6CB83E654DC")
    {
        Gleam_MakeType_AddField(sourceScene);
#ifdef GleamEngineEditor
        if constexpr (std::is_same_v<TFieldTransferrer, EditorUISerializer>)
        {
            std::string name = instanceScene.GetName();
            Gleam_MakeType_AddField(name);
        }
#endif
    }
}