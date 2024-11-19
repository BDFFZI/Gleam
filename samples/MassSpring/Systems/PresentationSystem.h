#pragma once
#include "LightECS/Runtime/System.hpp"
#include "LightGL/Runtime/Pipeline/GLCommandBuffer.h"
#include "LightGraphics/Runtime/CommandBuffer.h"

namespace Light
{
    struct BeginPresentationSystem : System<MinSystemOrder, MaxSystemOrder>
    {
        static void Start();
        static void Stop();
        static void Update();
    };

    struct EndPresentationSystem : System<BeginPresentationSystem, MaxSystemOrder>
    {
        static void Update();
    };

#define PresentationSystemGroup Light::BeginPresentationSystem,Light::EndPresentationSystem

    class Presentation
    {
    public:
        /**
         * 用于执行呈现命令的底层命令缓冲区。
         *
         * 该缓冲区由系统自动控制录制的启用和结束。
         * @return 
         */
        static GLCommandBuffer& GetPresentCommandBuffer() { return *presentCommandBuffer; }
        /**
         * 默认分配的公共子命令缓冲区，以方便用户快速绘制。
         *
         * 该缓冲区由系统自动控制录制的启用和结束，并会在呈现命令缓冲区执行前调用该缓冲区，
         * @return 
         */
        static CommandBuffer& GetCommandBuffer() { return *commandBuffer; }

    private:
        friend BeginPresentationSystem;
        friend EndPresentationSystem;
        inline static GLCommandBuffer* presentCommandBuffer = nullptr;
        inline static CommandBuffer* commandBuffer = nullptr;
    };
}
