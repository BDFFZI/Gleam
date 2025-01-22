#pragma once

#include "LightECS/Runtime/System.h"
#include "LightGL/Runtime/GLCommandBuffer.h"
#include "LightGraphics/Runtime/GCommandBuffer.h"
#include "LightEngine/Runtime/Engine.h"

namespace Light
{
    class PresentationSystem : public SystemGroup
    {
    public:
        PresentationSystem(): SystemGroup(SimulationSystem, OrderRelation::After)
        {
        }

        /**
         * 用于执行呈现命令的底层命令缓冲区。
         *
         * 该缓冲区由系统自动控制录制的启用和结束。
         * @return 
         */
        GLCommandBuffer& GetPresentCommandBuffer() const { return *presentCommandBuffer; }
        /**
         * 默认分配的公共子命令缓冲区，以方便用户快速绘制。
         *
         * 该缓冲区由系统自动控制录制的启用和结束，并会在呈现命令缓冲区执行前调用该缓冲区，
         * @return 
         */
        GCommandBuffer& GetCommandBuffer() const { return *commandBuffer; }

        void Start() override;
        void Stop() override;
        void Update() override;

    private:
        GLCommandBuffer* presentCommandBuffer = nullptr;
        GCommandBuffer* commandBuffer = nullptr;
    };
    Light_MakeSystem(PresentationSystem)
}