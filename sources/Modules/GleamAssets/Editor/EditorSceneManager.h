#pragma once
#include <filesystem>

#include "GleamAssets/Runtime/SceneManager.h"
#include "GleamECS/Runtime/Scene.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"
#include "GleamUtility/Runtime/Ranges.h"

namespace Gleam
{
    class EditorSceneManager
    {
    public:
        static void OpenScene(const std::filesystem::path& path);
        static void CloseScene(Scene& scene);
        static void SaveScene(Scene& scene);

    private:
        friend void OpenLastScene();

        inline static std::unordered_map<Scene*, std::filesystem::path> scenePaths = {};
        inline static std::filesystem::path lastScenePath = "";
    };

    //激活所有场景
    void StartScenes();
    Gleam_MakeSystemEvent(EditorSceneManager_RuntimeStartEvent, GlobalPostUpdateSystem, System::MaxOrder, Start, StartScenes)

    //清理并打开上次场景
    void OpenLastScene();
    Gleam_MakeSystemEvent(EditorSceneManager_EditorStartEvent, GlobalPostUpdateSystem, System::MaxOrder, Start, OpenLastScene)
}