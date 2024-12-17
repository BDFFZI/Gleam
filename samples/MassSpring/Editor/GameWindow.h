#pragma once
#include "../Public/UISystem.h"
#include "LightECS/Runtime/System.h"
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
        friend class GameWindowLogic;
        uint32_t lastWindowSize[2] = {0, 0};
        uint32_t windowSize[2] = {0, 0};
        ImTextureID textureID = {};

        void Start() override;
        void Stop() override;
        void Update() override;
    };
    inline GameWindow GameWindow = {};

    class GameWindowLogic : public System
    {
    public:
        GameWindowLogic(): System(nullptr, RightOrder)
        {
        }

    private:
        std::unique_ptr<RenderTexture> renderTexture = nullptr;
        
        void Update() override;
        void Stop() override;
    };
    inline GameWindowLogic GameWindowLogic = {};
}
