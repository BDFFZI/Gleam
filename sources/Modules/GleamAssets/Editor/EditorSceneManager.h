#pragma once
#include <filesystem>

#include "GleamAssets/Runtime/Scene/Scene.h"

namespace Gleam
{
    class EditorSceneManager
    {
    public:
        static Scene& OpenScene(const std::filesystem::path& path);
        static void CloseScene(Scene& scene);
        static void SaveScene(Scene& scene);
        static void ClearScene();

    private:
        friend class EditorSceneSystem;

        inline static std::unordered_map<Scene*, std::filesystem::path> sceneAssets = {};
        inline static std::filesystem::path currentScenePath = "";
    };
}