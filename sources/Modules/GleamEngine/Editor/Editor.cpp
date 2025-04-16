#include "Editor.h"

#include "GleamEngine/Runtime/Engine.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"

namespace Gleam
{
    void Editor::AddPlayEvent(const std::function<void()>& event, int order)
    {
        playEvents.emplace(order, event);
    }
    void Editor::AddStopEvent(const std::function<void()>& event, int order)
    {
        stopEvents.emplace(order, event);
    }
    void Editor_ReplaceRuntimeSystem()
    {
        Editor::runtimeSystems = std::move(Engine::runtimeSystems); //剥夺运行时引擎对全局系统的控制权
        Editor::editorSystems.AddGlobalSystemsToWorld(); //添加编辑器系统
    }
    void Editor_PlayPauseStopEngine()
    {
        //进入或退出运行时
        static bool lastIsPlaying = false;
        if (lastIsPlaying != Editor::GetIsPlaying())
        {
            if (Editor::GetIsPlaying())
            {
                for (auto& event : Editor::playEvents | std::views::values)
                    event();

                Editor::runtimeSystems.AddGlobalSystemsToWorld(); //添加运行时系统
            }
            else
            {
                World::Clear(); //重置世界
                Editor::editorSystems.AddGlobalSystemsToWorld(); //添加编辑器系统

                for (auto& event : Editor::stopEvents | std::views::values)
                    event();
            }
        }
        lastIsPlaying = Editor::GetIsPlaying();

        //约束运行时时间
        if (Editor::isPlaying)
            World::GetSystemAllocator().GetSystem<TimeSystem>().SetAutoStepTime(!Editor::GetIsPaused());
    }
}