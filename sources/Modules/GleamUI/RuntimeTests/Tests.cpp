#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Runtime/Engine.h"
#include "GleamUI/Runtime/UI.h"
#include "GleamUI/Runtime/UISystem.h"
#include "GleamWindow/Runtime/System/CursorSystem.h"
#include "GleamWindow/Runtime/System/InputSystem.h"
#include "GleamWindow/Runtime/Window.h"

using namespace Gleam;

class MySystem : public System
{
public:
    MySystem(): System(UISystem)
    {
    }

private:
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
        UI::DragFloat4x4("matrix", &matrix);

        //逻辑处理
        if (InputSystem->GetKeyDown(KeyCode::Esc))
            Engine::Stop();

        if (InputSystem->GetMouseButtonDown(MouseButton::Right))
        {
            CursorSystem->SetLockState(true);
            CursorSystem->SetVisible(false);
        }
        else if (InputSystem->GetMouseButtonUp(MouseButton::Right))
        {
            CursorSystem->SetLockState(false);
            CursorSystem->SetVisible(true);
        }
    }
};
Gleam_MakeSystemInstance(MySystem)
Gleam_AddSystems(MySystem)