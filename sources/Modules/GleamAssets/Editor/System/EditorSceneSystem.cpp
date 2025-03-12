#include "EditorSceneSystem.h"

#include "GleamAssets/Editor/EditorSceneManager.h"

namespace Gleam
{
    void EditorSceneSystem::Start()
    {
        if (!EditorSceneManager::currentScenePath.empty())
            EditorSceneManager::OpenScene(EditorSceneManager::currentScenePath);
    }
    void EditorSceneSystem::Stop()
    {
        EditorSceneManager::ClearScene();
    }
}