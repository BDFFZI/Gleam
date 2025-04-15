#include "Editor.h"

#include "GleamEngine/Runtime/Engine.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"

namespace Gleam
{
    void Editor_ReplaceRuntimeSystem()
    {
        Editor::runtimeSystems = std::move(Engine::runtimeSystems); //剥夺运行时引擎对全局系统的控制权
        Editor::editorSystems.AddGlobalSystemsToWorld(); //添加编辑器系统
    }
    void Editor_PlayPauseStopEngine()
    {
        static bool lastIsPlaying = false;
        if (lastIsPlaying != Editor::GetIsPlaying())
        {
            if (Editor::GetIsPlaying())
            {
                Editor::runtimeSystems.AddGlobalSystemsToWorld(); //添加运行时系统
            }
            else
            {
                World::Clear(); //重置世界
                Editor::editorSystems.AddGlobalSystemsToWorld(); //添加编辑器系统
            }
        }

        lastIsPlaying = Editor::GetIsPlaying();

        if (Editor::isPlaying)
        {
            World::GetSystemAllocator().GetSystem<TimeSystem>().SetAutoStepTime(!Editor::GetIsPaused());
        }
    }
}