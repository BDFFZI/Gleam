#include "LightECS/Runtime/World.h"
#include "LightEngine/Runtime/Engine.h"
#include "LightUI/Runtime/UI.h"
#include "LightUI/Runtime/UISystem.h"
#include "LightWindow/Runtime/System/CursorSystem.h"
#include "LightWindow/Runtime/System/InputSystem.h"
#include "LightWindow/Runtime/Window.h"

using namespace Light;

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
            float4::Yellow(), float4::Magenta(), float4::LightRed(),
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
Light_MakeSystemInstance(MySystem)
Light_AddSystems(MySystem)