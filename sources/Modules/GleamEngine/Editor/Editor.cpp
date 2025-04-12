#include "Editor.h"

#include "GleamEngine/Runtime/Engine.h"

namespace Gleam
{
    bool& Editor::IsPlaying()
    {
        return isPlaying;
    }
    void Editor_ReplaceRuntimeSystem()
    {
        Editor::runtimeSystems = std::move(Engine::runtimeSystems); //剥夺运行时引擎对全局系统的控制权
        Editor::editorSystems.AddGlobalSystemsToWorld(); //添加编辑器系统
    }
    void Editor_PlayOrStopEngine()
    {
        static bool lastIsPlaying = false;
        if (lastIsPlaying != Editor::IsPlaying())
        {
            if (Editor::IsPlaying())
            {
                Editor::runtimeSystems.AddGlobalSystemsToWorld(); //添加运行时系统
            }
            else
            {
                World::Clear(); //重置世界
                Editor::editorSystems.AddGlobalSystemsToWorld(); //添加编辑器系统
            }
        }

        lastIsPlaying = Editor::IsPlaying();
    }
}