#pragma once
#include <string>
#include <unordered_map>
#include <uuid.h>

#include "GleamAssets/Runtime/Configuration.h"
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    struct SceneSetting
    {
        inline static std::string startSceneName = {};
        inline static std::unordered_map<std::string, uuids::uuid> sceneNameMapping = {};
    };
    Gleam_MakeTypeWithID(SceneSetting, "056C22B3-4CF6-4771-818B-30F67FE75DDD")
    {
        Gleam_MakeType_AddField(startSceneName);
        Gleam_MakeType_AddField(sceneNameMapping);
    }
    Gleam_MakeSetting("SceneSetting.asset", SceneSetting)
}