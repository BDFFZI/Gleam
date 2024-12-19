#pragma once
#include "LightECS/Runtime/System.h"
#include "LightEngine/Runtime/System/SimulationSystem.h"
#include "LightEngine/Runtime/System/UISystem.h"
#include "LightUI/Runtime/UI.h"

namespace Light
{
    class GameWindow : public System
    {
    public:
        GameWindow(): System(&UISystem)
        {
        }

    private:
        friend class GameWindowAssetsSystem;
        float2 windowSize[2] = {0, 0};
        float2 windowOrigin = 0;
        bool isWindowFocused[2] = {true, true};
        //由GameWindowAssetsSystem处理的资源
        ImTextureID textureID = {};
        std::unique_ptr<RenderTexture> renderTexture = nullptr;
        InputHandler inputHandler = {"Editor"};

        void Update() override;
    };
    inline GameWindow GameWindow = {};

    class GameWindowAssetsSystem : public System
    {
    public:
        GameWindowAssetsSystem(): System(nullptr, LeftOrder,SimulationSystem.order)
        {
        }

    private:
        void Update() override;
        void Stop() override;
    };
    inline GameWindowAssetsSystem GameWindowAssetsSystem = {};
}