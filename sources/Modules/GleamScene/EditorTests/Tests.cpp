#include "GleamEngine/Runtime/Engine.h"
#include "GleamMath/Runtime/Geometry/3D/Sphere.h"
#include "GleamScene/Editor/Asset/AssetDatabase.h"
#include "GleamScene/Editor/Asset/AssetImporter.h"
#include "GleamScene/Editor/System/ProjectWindow.h"

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