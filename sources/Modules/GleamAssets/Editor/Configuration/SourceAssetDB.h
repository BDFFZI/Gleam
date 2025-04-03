#pragma once
#include "GleamAssets/Runtime/Configuration.h"
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    struct SourceAssetDB
    {
        //原文件的最后修改时间，文件内容的hash
        inline static std::unordered_map<std::filesystem::path, std::tuple<int64_t, uuids::uuid>> assetModificationInfos = {};
    };
    Gleam_MakeTypeWithID(SourceAssetDB, "011ED01E-45BE-4705-969A-22D3FD10252B")
    {
        Gleam_MakeType_AddField(assetModificationInfos);
    }
    Gleam_MakeConfigurationWithAutoSave("Library/SourceAssetDB.json", SourceAssetDB)
}