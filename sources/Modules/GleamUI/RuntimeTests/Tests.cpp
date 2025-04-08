#include "GleamECS/Runtime/World/World.h"
#include "GleamEngine/Runtime/Engine.h"
#include "GleamUI/Runtime/UI.h"
#include "GleamUI/Runtime/UISystem.h"
#include "GleamWindow/Runtime/Library/Cursor.h"
#include "GleamWindow/Runtime/System/InputSystem.h"

using namespace Gleam;

class MySystem : public SystemT<UISystem>
{
    InputSystem* inputSystem = nullptr;
    ImTextureID textureID = {};
    std::unique_ptr<GTexture2D> texture = {};
    float4x4 matrix = {
        11, 12, 13, 14,
        21, 22, 23, 24,
        31, 32, 33, 34,
        41, 42, 43, 44,
    };

    void Start() override
    {
        inputSystem = &World::GetCurrentWorld().GetSystemAllocator().GetSystem<InputSystem>();
        float4 colors[] = {
            float4::White(), float4::Black(), float4::Red(),
            float4::Green(), float4::Blue(), float4::Gray(),
            float4::Yellow(), float4::Magenta(), float4::GleamRed(),
        };
        texture = std::make_unique<GTexture2D>(3, 3, VK_FORMAT_R32G32B32A32_SFLOAT, colors, sizeof(colors));
        textureID = UI::CreateTexture(*texture);
    }
    void Stop() override
    {
        UI::DeleteTexture(textureID);
        texture.reset();
    }

    void Update() override
    {
        ImGui::ShowDemoWindow();

        ImGui::Image(textureID, float2(50));
        if (ImGui::BeginPopupContextItem("texture"))
        {
            ImGui::Text("This a texture!");
            ImGui::EndPopup();
        }

        static char buffer[10] = {};
        static char buffer2[10] = {};
        ImGui::InputText("a", buffer, sizeof(buffer));
        ImGui::InputText("b", buffer2, sizeof(buffer2));

        ImGui::ArrowButton("ArrowButton", ImGuiDir_Right);

        UI::DragScalarMatrix("matrix", ImGuiDataType_Float, &matrix, 4, 4);

        std::unordered_map<std::string, float> map = {
            {"Test1/Test1", 2},
            {"Test1", 1},
            {"Test2", 3},
        };

        float floatValues[] = {1, 2, 3};
        ImGui::DragScalarN("DragScalarN", ImGuiDataType_Float, floatValues, std::size(floatValues));

        //逻辑处理
        Input& input = inputSystem->GetDefaultInput();
        if (input.GetKeyDown(KeyCode::Esc))
            Engine::Stop();

        if (input.GetMouseButtonDown(MouseButton::Right))
        {
            Cursor::SetLockState(true);
            Cursor::SetVisible(false);
        }
        else if (input.GetMouseButtonUp(MouseButton::Right))
        {
            Cursor::SetLockState(false);
            Cursor::SetVisible(true);
        }
    }
};
Gleam_MakeSystem(MySystem)

Gleam_AddRuntimeSystems(MySystem)
Gleam_Main