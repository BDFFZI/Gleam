#pragma once

#include "GleamECS/Runtime/Entity/Archetype.h"
#include "GleamEngine/Runtime/Engine.h"

namespace Gleam
{
    class Editor
    {
    public:
        static GlobalSystemAllocator& GetEditorSystems()
        {
            return editorSystems;
        }

        static bool& IsPlaying();

    private:
        friend void Editor_ReplaceRuntimeSystem();
        friend void Editor_PlayOrStopEngine();
        static inline GlobalSystemAllocator editorSystems;
        static inline GlobalSystemAllocator runtimeSystems;
        static inline bool isPlaying = false;
        static inline Entity editorTimeEntity;
    };

    void Editor_ReplaceRuntimeSystem();
    void Editor_PlayOrStopEngine();

#define Gleam_MakeEditorSystem(type) inline type* Global##type = ::Gleam::Editor::GetEditorSystems().MakeGlobalSystem<type>(Global##type);
}