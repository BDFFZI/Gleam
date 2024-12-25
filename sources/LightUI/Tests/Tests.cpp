#include "LightMath/Runtime/Graphics/Color.hpp"
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
    std::unique_ptr<Texture2D> texture = {};

    void Start() override
    {
        color colors[] = {
            color::White(), color::Black(), color::Red(),
            color::Green(), color::Blue(), color::Gray(),
            color::Yellow(), color::Magenta(), color::LightRed(),
        };
        texture = std::make_unique<Texture2D>(3, 3, VK_FORMAT_R32G32B32A32_SFLOAT, colors, sizeof(colors));
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

int main()
{
    Engine::Start();
    return 0;
}