#pragma once
#include <filesystem>

#include "GleamAssets/Runtime/SceneManager.h"
#include "GleamECS/Runtime/Scene.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"

namespace Gleam
{
    class EditorSceneManager
    {
    public:
        static void OpenScene(const std::filesystem::path& path);
        static void CloseScene(Scene& scene);
        static void SaveScene(Scene& scene);

    private:
        friend void EditorSceneManager_OpenLastScene();

        inline static std::unordered_map<Scene*, std::filesystem::path> scenePaths = {};
    };

    void EditorSceneManager_StartAllScenes();
    void EditorSceneManager_OpenLastScene();
}