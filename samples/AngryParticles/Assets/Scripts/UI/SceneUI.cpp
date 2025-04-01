#include "SceneUI.h"

#include "GleamAssets/Runtime/SceneManager.h"
#include "GleamUI/Runtime/UI.h"

void StartSceneUI::Update()
{
    if (ImGui::Button("Start Game"))
    {
        Gleam::SceneManager::LoadScene("Menu");
    }
    if (ImGui::Button("Exit Game"))
    {
        Gleam::Engine::Stop();
    }
}

void MenuSceneUI::Update()
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

void LevelSceneUI::Update()
{
    if (ImGui::Button("Back"))
    {
        Gleam::SceneManager::LoadScene("Menu");
    }
}