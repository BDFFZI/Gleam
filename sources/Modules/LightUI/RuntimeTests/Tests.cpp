#include <gtest/gtest.h>

#include "LightMath/Runtime/Graphics/Color.h"
#include "LightUI/Runtime/UI.h"
#include "LightUI/Runtime/UISystem.h"
#include "LightWindow/Runtime/Input.h"
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

    void Start() override
    {
        Color colors[] = {
            Color::White(), Color::Black(), Color::Red(),
            Color::Green(), Color::Blue(), Color::Gray(),
            Color::Yellow(), Color::Magenta(), Color::LightRed(),
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

        //逻辑处理
        if (Input->GetKeyDown(KeyCode::Esc))
            Engine::Stop();
    }
};
Light_MakeSystem(MySystem)
Light_AddSystems(MySystem)

TEST(UI, Main)
{
    Engine::Start();
}