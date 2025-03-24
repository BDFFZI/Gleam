#pragma once
#include <filesystem>
#include <imgui.h>

#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamEngine/Editor/System/EditorUISystem.h"

#include "GleamAssets/Editor/Asset/AssetImporter.h"
#include "GleamAssets/Editor/Asset/AssetDatabase.h"

namespace Gleam
{
    class ProjectWindow : public System
    {
    public:
        static void AddDirectoryMenu(const std::string& name, const std::function<void()>& action);
        static void AddFileMenu(const std::string& extension, const std::string& name, const std::function<void()>& action);

        static const std::filesystem::path& GetFileDrawing();
        static const std::filesystem::path& GetDirectoryDrawing();

        ProjectWindow(): System(GlobalEditorUISystem)
        {
        }

    private:
        inline static std::unordered_map<std::string, std::function<void()>> directoryMenus = {};
        inline static std::unordered_map<std::string, std::unordered_map<std::string, std::function<void()>>> fileMenus = {};
        inline static std::filesystem::path fileDrawing = "";
        inline static std::filesystem::path directoryDrawing = "";

        std::set<uuids::uuid> assetBundlesLoading = {};
        std::vector<std::tuple<std::filesystem::path, std::filesystem::path>> movingPaths = {};
        std::vector<std::filesystem::path> removingPaths = {};

        void DragDropMovePath(const std::filesystem::path& path);
        ImGuiID DrawRenamePopup(const std::filesystem::path& path);
        ImGuiID DrawDeletePopup(const std::filesystem::path& path);
        void ShowFile(const std::filesystem::path& path);
        void ShowDirectory(const std::filesystem::path& path);
        
        void Start() override;
        void Stop() override;
        void Update() override;
    };
    Gleam_MakeGlobalSystem(ProjectWindow)


    class JsonObjectImporter : public AssetImporter
    {
        void LoadAsset(const std::filesystem::path& path, uuids::uuid& assetBundleID) override;
    };

#define Gleam_AddProjectWindowDirectoryMenu(name,action) \
Gleam_MakeInitEvent(){::Gleam::ProjectWindow::AddDirectoryMenu(name,action);}

#define Gleam_AddProjectWindowFileMenu(extension,name,action) \
Gleam_MakeInitEvent(){::Gleam::ProjectWindow::AddFileMenu(extension,name,action);}

#define Gleam_MakeCreateAssetMenu(type,extension)\
void ProjectWindowMenu_Create##type##Asset()\
{\
std::filesystem::path path = ProjectWindow::GetDirectoryDrawing() / "New"#type##extension;\
AssetBundle& assetBundle = AssetBundle::Create();\
assetBundle.AddAsset(type{});\
AssetDatabase::Create(path, assetBundle);\
AssetBundle::Unload(assetBundle);\
}\
Gleam_MakeInitEvent(){\
::Gleam::ProjectWindow::AddDirectoryMenu("Create/"#type,ProjectWindowMenu_Create##type##Asset);\
::Gleam::AssetImporter::AddCustomImporter(extension, Type::CreateOrGet<JsonObjectImporter>());\
}
}