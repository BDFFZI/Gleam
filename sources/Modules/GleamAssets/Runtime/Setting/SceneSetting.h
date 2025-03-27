#pragma once
#include <string>
#include <unordered_map>
#include <uuid.h>

#include "GleamAssets/Runtime/SettingManager.h"
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    struct SceneSetting
    {
        std::string startSceneName;
        std::unordered_map<std::string, uuids::uuid> sceneNameMapping;
    };
    Gleam_MakeType(SceneSetting, "056C22B3-4CF6-4771-818B-30F67FE75DDD")
    {
        Gleam_MakeType_AddField(startSceneName);
        Gleam_MakeType_AddField(sceneNameMapping);
    }
    Gleam_AddSetting("SceneSetting.asset", SceneSetting)
}