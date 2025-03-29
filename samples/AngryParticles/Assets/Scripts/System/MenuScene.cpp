#include "MenuScene.h"

#include <imgui.h>

#include "GleamAssets/Runtime/SceneManager.h"

void MenuScene::Update()
{
    if (ImGui::Button("Level 1"))
    {
        Gleam::SceneManager::LoadScene("Level1");
    }
    if (ImGui::Button("Level 2"))
    {
        Gleam::SceneManager::LoadScene("Level2");
    }
    if (ImGui::Button("Level 3"))
    {
        Gleam::SceneManager::LoadScene("Level3");
    }
    if (ImGui::Button("Back"))
    {
        Gleam::SceneManager::LoadScene("Start");
    }
}