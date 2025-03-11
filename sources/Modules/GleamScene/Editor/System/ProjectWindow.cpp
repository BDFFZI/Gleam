#include "ProjectWindow.h"

#include <filesystem>
#include <imgui.h>

#include "GleamScene/Editor/Asset/AssetDatabase.h"
#include "GleamScene/Editor/Asset/AssetImporter.h"
#include "GleamEngine/Editor/System/InspectorWindow.h"
#include "GleamUI/Runtime/UI.h"
#include "GleamWindow/Runtime/Window.h"

namespace Gleam
{
    void ProjectWindow::AddDirectoryMenu(const std::string& name, const std::function<void()>& action)
    {
        directoryMenus.emplace(name, action);
    }
    void ProjectWindow::AddFileMenu(const std::string& extension, const std::string& name, const std::function<void()>& action)
    {
        fileMenus[extension].emplace(name, action);
    }

    const std::filesystem::path& ProjectWindow::GetFileDrawing()
    {
        return fileDrawing;
    }
    const std::filesystem::path& ProjectWindow::GetDirectoryDrawing()
    {
        return directoryDrawing;
    }

    void RemoveAssetInspector(const std::filesystem::path& path)
    {
        AssetBundle& assetBundle = AssetDatabase::GetAssetBundle(path);
        auto optionalTarget = GlobalInspectorWindow.GetTarget();
        if (optionalTarget.has_value() && assetBundle.GetAsset(optionalTarget->data).has_value())
            GlobalInspectorWindow.SetTarget(std::nullopt);
    }
    void ProjectWindow::ShowFile(const std::filesystem::path& path)
    {
        //获取路径信息
        const std::string fileName = path.filename().string();
        const std::string extension = path.extension().string();

        //导入器文件不显示
        if (path.extension() == ".meta")
            return;

        //不支持的文件类型，仅显示名称
        if (AssetDatabase::CanLoad(path) == false)
        {
            ImGui::Text(fileName.c_str());
            return;
        }

        ImGui::PushID(fileName.data());

        //资源标题UI，显示打开按钮、资源名称、导入器选择按钮
        const bool isUnfolding = ImGui::CollapsingHeader(std::format("##{}", fileName).data(), ImGuiTreeNodeFlags_AllowOverlap);
        ImGui::SameLine();
        if (ImGui::Button(fileName.data(), {ImGui::GetContentRegionAvail().x, 0}))
        {
            AssetImporter& assetImporter = AssetImporter::GetImporter(path);
            GlobalInspectorWindow.SetTarget(InspectorTarget{assetImporter});
        }

        //资源右键菜单
        if (ImGui::BeginPopupContextItem("FilePopup"))
        {
            if (AssetDatabase::HasLoaded(path))
            {
                if (ImGui::MenuItem("ReLoad"))
                    AssetDatabase::Reload(path);
                if (ImGui::MenuItem("Save"))
                    AssetDatabase::Save(path);
            }

            if (fileMenus.contains(extension))
            {
                fileDrawing = path;
                auto& menu = fileMenus[extension];
                UI::Menu(menu);
            }

            ImGui::EndPopup();
        }

        //显示资源信息
        if (isUnfolding)
        {
            //首次展开，需加载资源包到内存
            uuids::uuid assetBundleID = AssetDatabase::GetAssetBundleID(path);
            if (!assetBundlesLoading.contains(assetBundleID))
            {
                AssetDatabase::Load(path);
                assetBundlesLoading.insert(assetBundleID);
            }

            //显示资源包内容
            AssetBundle& assetBundle = AssetDatabase::GetAssetBundle(path);
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
        else if (assetBundlesLoading.contains(AssetDatabase::GetAssetBundleID(path)))
        {
            //首次关闭，卸载资源包
            RemoveAssetInspector(path);
            AssetDatabase::Unload(path);
            assetBundlesLoading.erase(AssetDatabase::GetAssetBundleID(path));
        }

        ImGui::PopID();
    }
    void ProjectWindow::ShowDirectory(const std::filesystem::path& path)
    {
        std::string name = path.filename().string();
        const bool isUnfolding = ImGui::TreeNode(name.c_str());
        if (ImGui::BeginPopupContextItem())
        {
            directoryDrawing = path;
            fileDrawing = "";
            UI::Menu(directoryMenus);
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
        //除了结束时还未卸载的资源包
        for (auto assetBundleID : assetBundlesLoading)
            AssetBundle::UnLoad(AssetBundle::GetAssetBundle(assetBundleID));
    }

    void ProjectWindow::Update()
    {
        static bool lastIsFocused = false;
        if (lastIsFocused != Window::GetIsFocused())
        {
            if (Window::GetIsFocused())
                AssetDatabase::Refresh();
            lastIsFocused = Window::GetIsFocused();
        }

        if (ImGui::Begin("ProjectWindow", nullptr, ImGuiWindowFlags_MenuBar))
        {
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::MenuItem("Refresh"))
                    AssetDatabase::Refresh();

                ImGui::EndMenuBar();
            }

            ShowDirectory("Assets");
            if (std::filesystem::exists("StreamingAssets"))
                ShowDirectory("StreamingAssets");
        }

        ImGui::End();
    }
}