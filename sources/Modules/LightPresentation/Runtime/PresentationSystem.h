#pragma once

#include "LightECS/Runtime/System.h"
#include "LightGL/Runtime/GLCommandBuffer.h"
#include "LightGraphics/Runtime/GCommandBuffer.h"
#include "LightEngine/Runtime/System/UpdateSystem.h"
#include "LightWindow/Runtime/Window.h"

namespace Light
{
    /**
     * 将最终数据可视化输出到外部呈现设备中
     */
    class PresentationSystem : public SystemGroup
    {
    public:
        PresentationSystem(): SystemGroup(PostUpdateSystem, OrderRelation::After)
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

    private:
        friend class PresentationSystem;
        GLCommandBuffer* presentGLCommandBuffer = nullptr; //系统执行呈现命令用的缓冲区
        std::unique_ptr<GCommandBuffer> presentGCommandBuffer = nullptr; //预建的辅助命令缓冲区

        void Start() override;
        void Stop() override;
        void Update() override;
    };
    Light_MakeSystemInstance(PresentationSystem)
}