#include "GameUISystem.h"

#include <imgui.h>
#include <magic_enum.hpp>

#include "LogicSystem.h"
#include "GleamEngine/Runtime/Engine.h"

using namespace Gleam;

void GameUISystem::Update()
{
    //显示编辑模式
    const char* editModeOptions[] = {
        magic_enum::enum_name(static_cast<EditMode>(0)).data(),
        magic_enum::enum_name(static_cast<EditMode>(1)).data(),
        magic_enum::enum_name(static_cast<EditMode>(2)).data(),
        magic_enum::enum_name(static_cast<EditMode>(3)).data(),
    };
    ImGui::Combo("EditMode", reinterpret_cast<int*>(&LogicSystem.editMode), editModeOptions, std::size(editModeOptions));
    ImGui::Checkbox("Simulating", &LogicSystem.simulating);
    ImGui::SliderFloat("MassPointDrag", &LogicSystem.drag, 0.0f, 1.0f);
    ImGui::DragFloat("MassPointMass", &LogicSystem.mass, 0.1f, 0.0f);
    ImGui::DragFloat("SpringElasticity", &LogicSystem.elasticity, 1.0f, 0.0f);
    ImGui::SliderFloat("ColliderFriction", &LogicSystem.colliderFriction, 0.0f, 1.0f);
    ImGui::SliderFloat("ColliderElasticity", &LogicSystem.colliderElasticity, 0.0f, 1.0f);
    ImGui::Checkbox("Test", &LogicSystem.test);
    //退出游戏按钮
    if (ImGui::Button("ExitGame"))
        Engine::Stop();

#ifdef GleamEngineEditor
    //显示鼠标位置
    ImGui::DragFloat2("MousePositionWS", LogicSystem.mousePositionWS.data);
    //显示鼠标所在的点
    int pointID = static_cast<int>(LogicSystem.coveringPoint);
    ImGui::DragInt("CoveringPoint", &pointID);
#endif
}