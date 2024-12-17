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
        uint32_t lastWindowSize[2] = {0, 0};
        std::unique_ptr<RenderTexture> renderTexture = nullptr;
        ImTextureID textureID = {};

        void Start() override;
        void Stop() override;
        void Update() override;
    };
}
