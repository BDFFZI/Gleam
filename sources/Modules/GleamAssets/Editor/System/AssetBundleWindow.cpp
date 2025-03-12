#include "AssetBundleWindow.h"

#include <imgui.h>

#include "GleamEngine/Editor/System/InspectorWindow.h"
#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"

namespace Gleam
{
    void AssetBundleWindow::Update()
    {
        if (ImGui::Begin("AssetBundleWindow"))
        {
            for (AssetBundle& assetBundle : AssetBundle::GetAllAssetBundles())
            {
                if (ImGui::CollapsingHeader(to_string(assetBundle.GetID()).data()))
                {
                    for (const Asset& asset : assetBundle.GetAssets())
                    {
                        if (ImGui::Button(std::to_string(asset.GetID()).c_str()))
                        {
                            GlobalInspectorWindow.SetTarget(InspectorTarget{
                                asset.GetObject(),
                                Type::GetType(asset.GetTypeID()).value().get().GetIndex()
                            });
                        }
                    }
                }
            }
        }

        ImGui::End();
    }
}