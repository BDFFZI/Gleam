#pragma once
#include <filesystem>

#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Editor/System/EditorUISystem.h"
#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"

namespace Gleam
{
    class ProjectWindow : public System
    {
    public:
        static void AddPopupMenuItems(const std::string& name, const std::function<void()>& action);
        static const std::filesystem::path& GetFileDrawing();
        static const std::filesystem::path& GetDirectoryDrawing();
        
        ProjectWindow(): System(GlobalEditorUISystem)
        {
        }

    private:
        inline static std::unordered_map<std::string, std::function<void()>> menuItems = {};
        inline static std::filesystem::path fileDrawing = "";
        inline static std::filesystem::path directoryDrawing = "";
        inline static std::set<uuids::uuid> assetBundlesLoading = {};

        static void ShowFile(const std::filesystem::path& path);
        static void ShowDirectory(const std::filesystem::path& path);

        void Start() override;
        void Stop() override;
        void Update() override;
    };
    Gleam_MakeGlobalSystem(ProjectWindow)

#define Gleam_MakeProjectWindowMenu(name,action) \
Gleam_MakeInitEvent(){ProjectWindow::AddPopupMenuItems(name,action);}
}