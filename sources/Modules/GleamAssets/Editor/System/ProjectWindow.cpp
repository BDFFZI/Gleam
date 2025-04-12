#include "ProjectWindow.h"

#include <filesystem>
#include <imgui.h>

#include "GleamAssets/Editor/AssetDatabase/AssetDatabase.h"
#include "GleamAssets/Editor/AssetDatabase/AssetImporter.h"
#include "GleamEngine/Editor/System/InspectorWindow.h"
#include "GleamPersistence/Runtime/Resources.h"
#include "GleamUI/Runtime/UI.h"
#include "GleamUtility/Runtime/File.h"
#include "GleamWindow/Runtime/Window.h"

namespace Gleam
{
    void ProjectWindow::MakeDirectoryMenu(const std::string& name, const std::function<void()>& action)
    {
        directoryMenus.emplace(name, action);
    }
    void ProjectWindow::MakeFileMenu(const std::string& extension, const std::string& name, const std::function<void()>& action)
    {
        fileMenus[extension].emplace(name, action);
    }
    void ProjectWindow::MakeFileRenameEvent(const std::string& extension, const std::function<void(std::filesystem::path, std::filesystem::path)>& action)
    {
        fileRenameEvent.emplace(extension, action);
    }
    void ProjectWindow::MakeFileDeleteEvent(const std::string& extension, const std::function<void(std::filesystem::path)>& action)
    {
        fileDeleteEvent.emplace(extension, action);
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
    ImGuiID ProjectWindow::DrawRenamePopup(const std::filesystem::path& path)
    {
        static char buffer[64];

        std::string id = path.string() + "Rename";
        if (ImGui::BeginPopup(id.c_str()))
        {
            if (buffer[0] == 0)
            {
                std::string oldName = path.stem().string();
                std::memcpy(buffer, oldName.data(), oldName.size());
            }

            ImGui::InputText("##NewName", buffer, sizeof(buffer));
            if (ImGui::Button("Confirm") && std::strlen(buffer) != 0)
            {
                std::string newPath = path.string();
                String::Replace(newPath, path.stem().string(), buffer);
                movingPaths.emplace_back(path, newPath);

                for (char& i : buffer)
                    i = 0;
            }
            ImGui::EndPopup();
        }
        return ImGui::GetID(id.c_str());
    }
    ImGuiID ProjectWindow::DrawDeletePopup(const std::filesystem::path& path)
    {
        std::string id = path.string() + "Delete";
        if (ImGui::BeginPopup(id.c_str()))
        {
            if (ImGui::Button("Confirm"))
            {
                removingPaths.emplace_back(path);
            }

            ImGui::EndPopup();
        }
        return ImGui::GetID(id.c_str());
    }
    void ProjectWindow::DrawUniversalMenu(const std::filesystem::path& path)
    {
        if (ImGui::MenuItem("OpenInExplorer"))
        {
            File::OpenInExplorer(path);
        }
        if (ImGui::MenuItem("OpenInAssociatedApp"))
        {
            File::OpenInAssociatedApp(path);
        }
    }

    void ProjectWindow::ShowFile(const std::filesystem::path& path)
    {
        //获取路径信息
        const std::string fileName = path.filename().string();
        const std::string extension = path.extension().string();
        const bool canLoad = AssetDatabase::CanLoad(path);

        //导入器文件不显示
        if (path.extension() == ".meta")
            return;

        //名称显示
        bool isUnfolding;
        if (canLoad)
        {
            isUnfolding = ImGui::TreeNodeEx(
                path.filename().string().c_str(),
                ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth);
        }
        else
        {
            isUnfolding = false;
            ImGui::Selectable(path.filename().string().c_str());
        }

        //检视功能
        if (canLoad)
        {
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                AssetImporter& assetImporter = AssetImporter::GetImporter(path);
                GlobalInspectorWindow->SetMajorTarget(InspectorTarget{assetImporter});
            }
        }
        else
        {
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                File::OpenInAssociatedApp(path);
        }

        DragDropMovePath(path); //拖拽移动功能
        ImGuiID renamePopup = DrawRenamePopup(path);
        ImGuiID deletePopup = DrawDeletePopup(path);

        //右键菜单
        if (ImGui::BeginPopupContextItem())
        {
            //自定义菜单
            if (fileMenus.contains(extension))
            {
                fileDrawing = path;
                auto& menu = fileMenus[extension];
                UI::Menu(menu);
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Delete"))
            {
                ImGui::OpenPopup(deletePopup);
            }
            if (ImGui::MenuItem("Rename"))
            {
                ImGui::OpenPopup(renamePopup);
            }
            if (canLoad)
            {
                if (AssetDatabase::HasLoaded(path))
                {
                    if (ImGui::MenuItem("ReLoad"))
                        AssetDatabase::Reload(path);
                    if (ImGui::MenuItem("Save"))
                        AssetDatabase::Save(path);
                }
            }

            ImGui::Separator();

            DrawUniversalMenu(path);

            ImGui::EndPopup();
        }

        //显示资源信息
        if (canLoad)
        {
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
                        GlobalInspectorWindow->SetMajorTarget(InspectorTarget{
                            assetSlot.GetAsset().GetObjectPtr(),
                            assetSlot.GetAsset().GetObjectType().GetIndex()
                        });
                    }
                    EditorUI::SetDragDropObject(
                        assetSlot.GetAsset().GetObjectPtr(),
                        assetSlot.GetAsset().GetObjectType().GetIndex()
                    );
                }

                ImGui::TreePop();
            }
            else if (assetBundlesLoading.contains(assetBundleID))
            {
                //首次关闭，卸载资源包
                Resources::Unload(AssetBundle::GetAssetBundle(assetBundleID));
                assetBundlesLoading.erase(assetBundleID);
            }
        }
    }
    void ProjectWindow::ShowDirectory(const std::filesystem::path& path)
    {
        if (!exists(path))
            return;

        //名称显示
        const bool isUnfolding = ImGui::TreeNodeEx(
            path.filename().string().c_str(),
            ImGuiTreeNodeFlags_SpanAvailWidth
        );

        DragDropMovePath(path); //拖拽移动功能
        ImGuiID renamePopup = DrawRenamePopup(path);
        ImGuiID deletePopup = DrawDeletePopup(path);

        //右键菜单
        if (ImGui::BeginPopupContextItem())
        {
            //自定义菜单
            directoryDrawing = path;
            fileDrawing = "";
            UI::Menu(directoryMenus);

            if (ImGui::BeginMenu("Create"))
            {
                if (ImGui::MenuItem("Folder"))
                    AssetDatabase::CreateFolder(path / "NewFolder");
                ImGui::EndMenu();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Delete"))
            {
                ImGui::OpenPopup(deletePopup);
            }
            if (ImGui::MenuItem("Rename"))
            {
                ImGui::OpenPopup(renamePopup);
            }

            ImGui::Separator();

            DrawUniversalMenu(path);

            ImGui::EndPopup();
        }

        //显示子文件
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

    void ProjectWindow::Stop()
    {
        //回收结束时还未卸载的资源包
        for (auto assetBundleID : assetBundlesLoading)
            Resources::Unload(AssetBundle::GetAssetBundle(assetBundleID));
        assetBundlesLoading.clear();
    }
    void ProjectWindow::Update()
    {
        //根据窗口焦点状态刷新资源文件夹
        static bool lastIsFocused = false;
        if (lastIsFocused != Window::GetIsFocused())
        {
            if (Window::GetIsFocused())
                AssetDatabase::Refresh("Assets");
            lastIsFocused = Window::GetIsFocused();
        }

        if (ImGui::Begin("ProjectWindow", nullptr, ImGuiWindowFlags_MenuBar))
        {
            //窗口菜单
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::MenuItem("Refresh"))
                    AssetDatabase::Refresh("Assets");

                ImGui::EndMenuBar();
            }

            //绘制文件夹
            ShowDirectory("Assets"); //默认资源文件
            ShowDirectory("ProjectSettings"); //部分功能依赖的配置文件
            ImGui::Separator();
            ShowDirectory("Library"); //项目缓存文件
            ShowDirectory("StreamingAssets"); //来自库的原始资源文件
        }
        ImGui::End();

        //处理文件结构性变化
        for (auto& movingPath : movingPaths)
        {
            std::string extension = std::get<0>(movingPath).extension().string();
            if (fileRenameEvent.contains(extension))
                fileRenameEvent[extension](std::get<0>(movingPath), std::get<1>(movingPath));

            AssetDatabase::Move(std::get<0>(movingPath), std::get<1>(movingPath));
        }
        movingPaths.clear();
        for (auto& removingPath : removingPaths)
        {
            if (!is_directory(removingPath))
            {
                std::string extension = removingPath.extension().string();
                if (fileDeleteEvent.contains(extension))
                    fileDeleteEvent[extension](removingPath);

                uuids::uuid assetBundleID = AssetDatabase::GetAssetBundleID(removingPath);
                if (assetBundlesLoading.contains(assetBundleID))
                {
                    Resources::Unload(AssetBundle::GetAssetBundle(assetBundleID));
                    assetBundlesLoading.erase(assetBundleID);
                }
            }

            AssetDatabase::Delete(removingPath);
        }
        removingPaths.clear();
    }
}