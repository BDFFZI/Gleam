#include "Editor.h"

#include "GleamECS/Runtime/World/World.h"
#include "GleamEngine/Runtime/Engine.h"

namespace Gleam
{
    void Editor::AddEditorSystems(const std::initializer_list<std::reference_wrapper<System>> systems)
    {
        editorSystems.insert(editorSystems.end(), systems.begin(), systems.end());
    }
    void Editor::AddEditorOnlySystems(const std::initializer_list<std::reference_wrapper<System>> systems)
    {
        editorOnlySystems.insert(editorOnlySystems.end(), systems.begin(), systems.end());
    }
    bool& Editor::IsPlaying()
    {
        return isPlaying;
    }
    void Editor_InterceptRuntimeSystem()
    {
        for (auto system : Engine::runtimeSystems)
            Engine::GetMainWorld().RemoveSystem(*system); //撤销运行时系统
        for (auto system : Editor::editorSystems)
            Engine::GetMainWorld().RemoveSystem(*system);
        for (auto system : Editor::editorOnlySystems)
            Engine::GetMainWorld().RemoveSystem(*system);
    }
    void Editor_PlayOrStopEngine()
    {
        static bool lastIsPlaying = false;
        if (lastIsPlaying != Editor::IsPlaying())
        {
            if (Editor::IsPlaying())
            {
                for (auto system : Engine::runtimeSystems)
                    Engine::GetMainWorld().AddSystem(*system);
                for (auto system : Editor::editorOnlySystems)
                    Engine::GetMainWorld().AddSystem(*system);
            }
            else
            {
                World::Clear();

                for (auto system : Editor::editorSystems)
                    Engine::GetMainWorld().AddSystem(*system);
                for (auto system : Editor::editorOnlySystems)
                    Engine::GetMainWorld().AddSystem(*system);
            }
        }

        lastIsPlaying = Editor::IsPlaying();
    }
}