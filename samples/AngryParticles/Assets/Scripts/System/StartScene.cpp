#include "StartScene.h"

#include <imgui.h>

#include "GleamAssets/Runtime/SceneManager.h"

void StartSceneUI::Update()
{
    if (ImGui::Button("Start Game"))
    {
        Gleam::SceneManager::LoadScene("MenuScene");
    }
    if (ImGui::Button("Exit Game"))
    {
        Gleam::Engine::Stop();
    }
}