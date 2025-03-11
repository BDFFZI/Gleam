#include "ProjectWindow.h"

#include <filesystem>
#include <imgui.h>

#include "GleamAssets/Editor/Asset/AssetDatabase.h"
#include "GleamAssets/Editor/Asset/AssetMeta.h"
#include "GleamEngine/Editor/System/InspectorWindow.h"
#include "GleamUI/Runtime/UI.h"

namespace Gleam
{
    void ProjectWindow::AddPopupMenuItems(const std::string& name, const std::function<void()>& action)
    {
        menuItems.emplace(name, action);
    }
    const std::filesystem::path& ProjectWindow::GetFileDrawing()
    {
        return fileDrawing;
    }
    const std::filesystem::path& ProjectWindow::GetDirectoryDrawing()
    {
        return directoryDrawing;
    }

    void ProjectWindow::ShowFile(const std::filesystem::path& path)
    {
        if (path.extension() == ".meta")
            return;

        const std::string pathStr = path.string();
        const std::string fileName = path.filename().string();
        if (AssetDatabase::CanLoad(pathStr) == false)
            ImGui::Text(fileName.c_str()); //未知类型文件，仅显示名称
        else
        {
            ImGui::PushID(pathStr.c_str());

            const bool isUnfolding = ImGui::CollapsingHeader(std::format("##{}", fileName).data(), ImGuiTreeNodeFlags_AllowOverlap);
            ImGui::SameLine();

            //显示资源元信息
            if (ImGui::Button(fileName.data(), {ImGui::GetContentRegionAvail().x, 0}))
            {
                static std::unique_ptr<AssetMeta> assetMetaInspecting;
                assetMetaInspecting = AssetMeta::GetMeta(path);
                GlobalInspectorWindow.SetTarget(InspectorTarget{*assetMetaInspecting});
            }
            //右键菜单
            if (ImGui::BeginPopupContextItem("FilePopup"))
            {
                if (ImGui::MenuItem("ReLoad"))
                    AssetDatabase::Load(path, true);
                if (ImGui::MenuItem("UnLoad"))
                {
                    AssetBundle& assetBundle = AssetDatabase::Load(path);
                    auto optionalTarget = GlobalInspectorWindow.GetTarget();
                    if (optionalTarget.has_value() && assetBundle.GetAsset(optionalTarget->data).has_value())
                        GlobalInspectorWindow.SetTarget(std::nullopt);
                    AssetDatabase::UnLoad(path);
                }
                if (ImGui::MenuItem("Save"))
                    AssetDatabase::Save(path);

                ImGui::EndPopup();
            }

            if (isUnfolding)
            {
                //显示资源包内容
                AssetBundle& assetBundle = AssetDatabase::Load(path);
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

                assetBundlesLoading.insert(assetBundle.GetID());
            }
            else if (assetBundlesLoading.contains(AssetDatabase::GetAssetBundleID(path)))
            {
                //卸载资源包
                AssetBundle& assetBundle = AssetDatabase::Load(path);
                auto optionalTarget = GlobalInspectorWindow.GetTarget();
                if (optionalTarget.has_value() && assetBundle.GetAsset(optionalTarget->data).has_value())
                    GlobalInspectorWindow.SetTarget(std::nullopt);
                AssetDatabase::UnLoad(path);

                assetBundlesLoading.erase(assetBundle.GetID());
            }

            ImGui::PopID();
        }
    }
    void ProjectWindow::ShowDirectory(const std::filesystem::path& path)
    {
        std::string name = path.filename().string();
        const bool isUnfolding = ImGui::TreeNode(name.c_str());
        if (ImGui::BeginPopupContextItem())
        {
            directoryDrawing = path;
            fileDrawing = "";
            UI::Menu(menuItems);
            ImGui::EndPopup();
        }

        if (isUnfolding)
        {
            //统计目录下元素
            std::vector<std::filesystem::path> directories = {};
            std::vector<std::filesystem::path> files = {};
            for (const auto& child : std::filesystem::directory_iterator{path})
                if (child.is_directory())directories.emplace_back(child.path());
                else files.emplace_back(child.path());

            //先显示目录
            for (auto& directory : directories)
                ShowDirectory(directory);

            //再显示文件
            directoryDrawing = path;
            for (auto& file : files)
                ShowFile(file);

            ImGui::TreePop();
        }
    }
    void ProjectWindow::Start()
    {
        if (!std::filesystem::exists("Assets"))
            std::filesystem::create_directory("Assets");
    }
    void ProjectWindow::Stop()
    {
        for (auto assetBundleID : assetBundlesLoading)
            AssetBundle::UnLoad(AssetBundle::GetAssetBundle(assetBundleID));
    }

    void ProjectWindow::Update()
    {
        ImGui::Begin("ProjectWindow");

        ShowDirectory("Assets");
        if (std::filesystem::exists("StreamingAssets"))
            ShowDirectory("StreamingAssets");

        ImGui::End();
    }
}