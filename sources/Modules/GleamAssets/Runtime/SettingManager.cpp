#include "SettingManager.h"

#include "GleamAssets/Editor/Asset/AssetDatabase.h"
#include "GleamPersistence/Runtime/Resources.h"
#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"

namespace Gleam
{
    void SettingManager::MakeSetting(std::string_view name, const Type& type)
    {
        settings.emplace(name, &type);
    }
}