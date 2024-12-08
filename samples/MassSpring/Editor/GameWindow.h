#pragma once
#include "../Systems/LogicSystem.h"

namespace Light
{
    class GameWindow : public SystemT<UISystemGroup>
    {
    public:
        static void Start();
        static void Update();

    private:
        inline static uint32_t windowSize[2];
        inline static std::unique_ptr<RenderTexture> renderTexture = nullptr;
    };
}
