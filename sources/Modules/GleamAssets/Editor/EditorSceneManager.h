#pragma once
#include <filesystem>

#include "GleamECS/Runtime/Scene.h"

namespace Gleam
{
    class EditorSceneManager
    {
    public:
        static Scene& OpenScene(const std::filesystem::path& path);
        static void CloseScene(Scene& scene);
        static void SaveScene(Scene& scene);

    private:
        inline static std::unordered_map<Scene*, std::filesystem::path> sceneAssets = {};
    };
}