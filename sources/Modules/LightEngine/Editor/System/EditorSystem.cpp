#include "EditorSystem.h"

#include "LightECS/Runtime/World.h"
#include "LightEngine/Editor/Editor.h"

namespace Light
{
    void EditorSystem::Update()
    {
        if (lastIsPlaying != Editor::IsPlaying())
        {
            if (Editor::IsPlaying())
            {
                for (auto* system : Engine::RuntimeSystems())
                {
                    World::AddSystem(system);
                }
            }
            else
            {
                for (auto* system : Engine::RuntimeSystems())
                {
                    World::RemoveSystem(system);
                }
            }
        }

        lastIsPlaying = Editor::IsPlaying();
    }
}