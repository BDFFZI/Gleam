#pragma once
#include "GleamAssets/Runtime/Asset/PersistentEntity.h"
#include "GleamAssets/Runtime/Asset/BasicSceneInfo.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/EditorUI/EditorUISerializer.h"
#endif

namespace Gleam
{
    struct ScenePrefab
    {
        std::weak_ptr<BasicSceneInfo> sourceScene; //只需要一个对场景资源包的依赖即可，BasicSceneInfo是场景资源包必有的资源，故选择其作为引用
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