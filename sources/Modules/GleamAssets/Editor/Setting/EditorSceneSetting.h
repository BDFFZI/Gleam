#pragma once
#include <filesystem>

#include "GleamAssets/Runtime/SettingManager.h"
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    struct EditorSceneSetting
    {
        inline static std::string lastScenePath = "";
    };
    Gleam_MakeTypeWithID(EditorSceneSetting, "28BC4368-5EF1-4C14-8AC7-E942F986F327")
    {
        Gleam_MakeType_AddField(lastScenePath);
    }
    Gleam_MakeSetting("EditorSceneSetting.asset", EditorSceneSetting)
}