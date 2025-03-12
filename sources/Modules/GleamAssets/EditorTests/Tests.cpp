#include "GleamMath/Runtime/Geometry/3D/Sphere.h"
#include "GleamAssets/Editor/Asset/AssetDatabase.h"
#include "GleamAssets/Editor/Asset/AssetImporter.h"
#include "GleamAssets/Editor/System/ProjectWindow.h"

Gleam_Main

using namespace Gleam;

void ProjectWindow_CreateTestAsset()
{
    auto path = ProjectWindow::GetDirectoryDrawing() / "TestAsset.asset";

    AssetBundle& assetBundle = AssetBundle::Create();
    {
        assetBundle.AddAsset(Sphere{{1, 2, 3}, 4});
        AssetDatabase::Save(path, assetBundle);
    }
    AssetBundle::UnLoad(assetBundle);
}

Gleam_MakeProjectDirectoryMenu("Create/TestAsset", ProjectWindow_CreateTestAsset)