#pragma once
#include <filesystem>

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
        friend class EditorSceneManager_EditorEvent;

        inline static std::unordered_map<Scene*, std::filesystem::path> scenePaths = {};
        inline static std::filesystem::path lastScenePath = "";
    };

    class EditorSceneManager_EditorEvent : public System
    {
    public:
        EditorSceneManager_EditorEvent(): System(GlobalPostUpdateSystem, MaxOrder, MaxOrder)
        {
        }

    private:
        void Start() override;
    };
    Gleam_MakeGlobalSystem(EditorSceneManager_EditorEvent)
}