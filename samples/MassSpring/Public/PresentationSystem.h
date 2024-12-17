#pragma once
#include "LightECS/Runtime/System.h"
#include "LightGL/Runtime/Pipeline/GLCommandBuffer.h"
#include "LightGraphics/Runtime/CommandBuffer.h"

namespace Light
{
    class PresentationSystem : public SystemGroup
    {
    public:
        PresentationSystem(): SystemGroup(nullptr, MiddleOrder, RightOrder)
        {
        }

        /**
         * 本次呈现是否有效
         *
         * 当呈现无效时，依然可以正常使用相关资源进行渲染，这样可以保证一定的代码兼容性。但由于呈现无效，相关代码的执行最终也是无效的，若追求效率，应检查该值。
         * @return 
         */
        bool CanPresent() const { return canPresent; }
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
        CommandBuffer& GetCommandBuffer() const { return *commandBuffer; }

        void Start() override;
        void Stop() override;
        void Update() override;

    private:
        bool canPresent = false;
        GLCommandBuffer* presentCommandBuffer = nullptr;
        CommandBuffer* commandBuffer = nullptr;
    };
    inline PresentationSystem PresentationSystem = {};
}
