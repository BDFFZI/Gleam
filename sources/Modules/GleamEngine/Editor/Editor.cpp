#include "Editor.h"

#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Runtime/Engine.h"

namespace Gleam
{
    std::vector<std::reference_wrapper<System>>& Editor::EditorSystems()
    {
        return editorSystems;
    }
    bool& Editor::IsPlaying()
    {
        return isPlaying;
    }

    void Editor_ReplaceRuntimeSystem()
    {
        for (auto system : Engine::RuntimeSystems())
            World::RemoveSystem(system);
        World::FlushSystemQueue();
        for (auto system : Editor::EditorSystems())
            World::AddSystem(system);
    }
    void Editor_PlayOrStopEngine()
    {
        static bool lastIsPlaying = false;
        if (lastIsPlaying != Editor::IsPlaying())
        {
            if (Editor::IsPlaying())
            {
                for (auto system : Engine::RuntimeSystems())
                    World::AddSystem(system);
            }
            else
            {
                World::Clear();
                for (auto system : Editor::EditorSystems())
                    World::AddSystem(system);
            }
        }

        lastIsPlaying = Editor::IsPlaying();
    }
}