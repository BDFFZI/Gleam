#pragma once

#include "LightECS/Runtime/System.h"
#include "LightGL/Runtime/GLCommandBuffer.h"
#include "LightGraphics/Runtime/GCommandBuffer.h"
#include "LightEngine/Runtime/System/UpdateSystem.h"

namespace Light
{
    /**
     * 将最终数据可视化输出到外部呈现设备中
     */
    class PresentationSystem : public SystemGroup
    {
    public:
        //通过修改该变量可以调整Presentation初始化Graphics库的方式，UI不需要，但Render需要
        inline static std::function CreateGraphicsConfig = [] { return std::make_unique<GraphicsConfig>(); };
        
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
        GLCommandBuffer* presentGLCommandBuffer = nullptr;
        std::unique_ptr<GCommandBuffer> presentGCommandBuffer = nullptr;

        void Start() override;
        void Stop() override;
        void Update() override;
    };
    Light_MakeGlobalSystem(PresentationSystem)
}