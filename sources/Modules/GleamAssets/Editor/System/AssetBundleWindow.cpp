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
                    ImGui::PushID(&assetBundle);
                    for (AssetSlot& assetSlot : assetBundle.GetAssetSlots())
                    {
                        if (ImGui::Button(std::to_string(assetSlot.GetID()).c_str()))
                        {
                            GlobalInspectorWindow.SetTarget(InspectorTarget{
                                assetSlot.GetAsset().GetObjectPtr(),
                                assetSlot.GetAsset().GetObjectType().GetIndex()
                            });
                        }
                    }
                    ImGui::PopID();
                }
            }
        }

        ImGui::End();
    }
}