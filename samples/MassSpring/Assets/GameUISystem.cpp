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
    ImGui::Combo("EditMode", reinterpret_cast<int*>(&GlobalLogicSystem.editMode), editModeOptions, std::size(editModeOptions));
    ImGui::SliderFloat("SimulatedSpeed", &GlobalLogicSystem.simulatedSpeed, 0.0f, 3.0f);
    ImGui::SliderFloat("MassPointDrag", &GlobalLogicSystem.drag, 0.0f, 1.0f);
    ImGui::DragFloat("MassPointMass", &GlobalLogicSystem.mass, 0.1f, 0.0f);
    ImGui::DragFloat("SpringElasticity", &GlobalLogicSystem.elasticity, 1.0f, 0.0f);
    ImGui::SliderFloat("ColliderFriction", &GlobalLogicSystem.colliderFriction, 0.0f, 1.0f);
    ImGui::SliderFloat("ColliderElasticity", &GlobalLogicSystem.colliderElasticity, 0.0f, 1.0f);
    ImGui::Checkbox("Test", &GlobalLogicSystem.test);
    //退出游戏按钮
    if (ImGui::Button("ExitGame"))
        Engine::Stop();

#ifdef GleamEngineEditor
    //显示鼠标位置
    ImGui::DragFloat2("MousePositionWS", GlobalLogicSystem.mousePositionWS.data);
    //显示鼠标所在的点
    int pointID = static_cast<int>(GlobalLogicSystem.coveringParticle);
    ImGui::DragInt("CoveringPoint", &pointID);
#endif
}