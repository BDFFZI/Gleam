#pragma once
#include "GleamAssets/Runtime/Configuration.h"
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    struct ArtifactDB
    {
        //在Unity中，该文件包含了导入依赖的信息，Artifact文件的meta信息以及Artifact文件信息。
        //不过目前，本引擎将这些信息作为.meta与Artifact文件存在了一起。所以本文件目前无效。
    };
    Gleam_MakeType(ArtifactDB, "60B1C814-9CE3-4B50-883C-2B4ACC92F003")
    {
    }
    Gleam_MakeConfiguration("Library/ArtifactDB.json", ArtifactDB)
}