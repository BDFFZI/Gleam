#pragma once

#include "GleamEngine/Runtime/Engine.h"

namespace Gleam
{
    class Editor
    {
    public:
        static void AddPlayEvent(const std::function<void()>& event, int order = 0);
        static void AddStopEvent(const std::function<void()>& event, int order = 0);
        static GlobalSystemAllocator& GetEditorSystems()
        {
            return editorSystems;
        }

        static bool GetIsPlaying()
        {
            return isPlaying;
        }
        static bool GetIsPaused()
        {
            return isPaused;
        }
        static void SetIsPlaying(const bool isPlaying)
        {
            Editor::isPlaying = isPlaying;
        }
        static void SetIsPaused(const bool isPaused)
        {
            Editor::isPaused = isPaused;
        }

    private:
        friend void Editor_ReplaceRuntimeSystem();
        friend void Editor_PlayPauseStopEngine();
        static inline std::multimap<int, std::function<void()>> playEvents;
        static inline std::multimap<int, std::function<void()>> stopEvents;
        static inline GlobalSystemAllocator editorSystems;
        static inline GlobalSystemAllocator runtimeSystems;
        static inline bool isPlaying = false;
        static inline bool isPaused = false;
    };

    void Editor_ReplaceRuntimeSystem();
    void Editor_PlayPauseStopEngine();

#define Gleam_MakeEditorSystem(type) inline type* Global##type = ::Gleam::Editor::GetEditorSystems().MakeGlobalSystem<type>(Global##type);
}