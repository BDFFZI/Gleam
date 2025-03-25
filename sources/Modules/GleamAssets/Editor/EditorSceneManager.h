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


    //激活所有场景
    void EditorSceneManager_StartScenes();
    Gleam_MakeSystemEvent(EditorSceneManager_StartScenes, Start, GlobalPostUpdateSystem, System::MaxOrder)

    //清理并打开上次场景
    void EditorSceneManager_OpenLastScene();
    Gleam_MakeSystemEvent(EditorSceneManager_OpenLastScene, Start, GlobalPostUpdateSystem, System::MaxOrder)
}