#include "StartScene.h"

#include <imgui.h>

#include "GleamAssets/Runtime/SceneManager.h"

void StartSceneUI::Update()
{
    if (ImGui::Button("Start Game"))
    {
        Gleam::SceneManager::LoadScene(uuids::uuid::from_string("d334c836-256a-47f4-9f87-e15a1bdd17aa").value());
    }
    if (ImGui::Button("Exit Game"))
    {
        Gleam::Engine::Stop();
    }
}