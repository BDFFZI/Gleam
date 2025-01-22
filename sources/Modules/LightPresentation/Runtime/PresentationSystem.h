#pragma once

#include "LightECS/Runtime/System.h"
#include "LightGL/Runtime/GLCommandBuffer.h"
#include "LightGraphics/Runtime/GCommandBuffer.h"
#include "LightEngine/Runtime/System.h"

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
        GLCommandBuffer& GetPresentGLCommandBuffer() const { return *presentGLCommandBuffer; }
        /**
         * 默认分配的公共子命令缓冲区，以方便用户快速绘制。
         *
         * 该缓冲区由系统自动控制录制的启用和结束，并会在呈现命令缓冲区执行前调用该缓冲区，
         * @return 
         */
        GCommandBuffer& GetPresentGCommandBuffer() const { return *presentGCommandBuffer; }

        void Start() override;
        void Stop() override;
        void Update() override;

    private:
        GLCommandBuffer* presentGLCommandBuffer = nullptr;
        std::unique_ptr<GCommandBuffer> presentGCommandBuffer = nullptr;
    };
    Light_MakeSystem(PresentationSystem)
}