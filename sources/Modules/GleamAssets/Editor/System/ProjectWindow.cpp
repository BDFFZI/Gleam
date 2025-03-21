#include "ProjectWindow.h"

#include <filesystem>
#include <imgui.h>

#include "GleamAssets/Editor/Asset/AssetDatabase.h"
#include "GleamAssets/Editor/Asset/AssetImporter.h"
#include "GleamEngine/Editor/System/InspectorWindow.h"
#include "GleamPersistence/Runtime/Resources.h"
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

    void ProjectWindow::DragDropMovePath(const std::filesystem::path& path)
    {
        if (ImGui::BeginDragDropSource())
        {
            static std::filesystem::path draggingCache = {};
            draggingCache = path;
            ImGui::SetDragDropPayload(typeid(std::filesystem::path).name(), &draggingCache, sizeof(draggingCache));
            ImGui::EndDragDropSource();
        }
        if (is_directory(path) && ImGui::BeginDragDropTarget())
        {
            if (auto payload = ImGui::AcceptDragDropPayload(typeid(std::filesystem::path).name()))
            {
                auto sourcePath = *static_cast<std::filesystem::path*>(payload->Data);
                movingPaths.emplace_back(sourcePath, path / sourcePath.filename());
            }
            ImGui::EndDragDropTarget();
        }
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
        bool isUnfolding = ImGui::CollapsingHeader(std::format("##{}", fileName).data(), ImGuiTreeNodeFlags_AllowOverlap);
        ImGui::SameLine();
        if (ImGui::Button(fileName.data(), {ImGui::GetContentRegionAvail().x, 0}))
        {
            AssetImporter& assetImporter = AssetImporter::GetImporter(path);
            GlobalInspectorWindow.SetTarget(InspectorTarget{assetImporter});
        }
        DragDropMovePath(path);

        //资源右键菜单
        if (ImGui::BeginPopupContextItem("FilePopup"))
        {
            if (fileMenus.contains(extension))
            {
                fileDrawing = path;
                auto& menu = fileMenus[extension];
                UI::Menu(menu);
            }

            if (ImGui::MenuItem("Delete"))
            {
                AssetDatabase::Delete(path);
                isUnfolding = false;
            }

            if (AssetDatabase::HasLoaded(path))
            {
                if (ImGui::MenuItem("ReLoad"))
                    AssetDatabase::Reload(path);
                if (ImGui::MenuItem("Save"))
                    AssetDatabase::Save(path);
            }

            ImGui::EndPopup();
        }

        //显示资源信息
        uuids::uuid assetBundleID = AssetDatabase::GetAssetBundleID(path);
        if (isUnfolding)
        {
            //首次展开，需加载资源包到内存
            if (!assetBundlesLoading.contains(assetBundleID))
            {
                Resources::Load(assetBundleID);
                assetBundlesLoading.insert(assetBundleID);
            }

            //显示资源包内容
            AssetBundle& assetBundle = AssetBundle::GetAssetBundle(assetBundleID);
            for (AssetSlot& assetSlot : assetBundle.GetAssetSlots())
            {
                if (ImGui::Button(std::to_string(assetSlot.GetID()).c_str()))
                {
                    GlobalInspectorWindow.SetTarget(InspectorTarget{
                        assetSlot.GetAsset().GetObjectPtr(),
                        assetSlot.GetAsset().GetObjectType().GetIndex()
                    });
                }
                EditorUI::SetDragDropObject(
                    assetSlot.GetAsset().GetObjectPtr(),
                    assetSlot.GetAsset().GetObjectType().GetIndex()
                );
            }
        }
        else if (assetBundlesLoading.contains(assetBundleID))
        {
            //首次关闭，卸载资源包
            Resources::Unload(AssetBundle::GetAssetBundle(assetBundleID));
            assetBundlesLoading.erase(assetBundleID);
        }

        ImGui::PopID();
    }
    void ProjectWindow::ShowDirectory(const std::filesystem::path& path)
    {
        std::string name = path.filename().string();
        const bool isUnfolding = ImGui::TreeNode(name.c_str());
        DragDropMovePath(path);

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
        AssetDatabase::Refresh();
    }
    void ProjectWindow::Stop()
    {
        //除了结束时还未卸载的资源包
        for (auto assetBundleID : assetBundlesLoading)
            Resources::Unload(AssetBundle::GetAssetBundle(assetBundleID));
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

        for (auto& movingPath : movingPaths)
            AssetDatabase::Move(std::get<0>(movingPath), std::get<1>(movingPath));
        movingPaths.clear();
    }

    void JsonObjectImporter::LoadAsset(const std::filesystem::path& path, uuids::uuid& assetBundleID)
    {
        AssetBundle& assetBundle = AssetBundle::LoadJson(path.string(), true);
        assetBundleID = assetBundle.GetID();
    }
}