#pragma once

#include "PresentationSystem.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

namespace Gleam
{
    //通过修改该变量可以调整Presentation初始化Graphics库的方式，UI不需要，但Render需要
    inline static std::function CreateGraphicsConfig = [] { return std::make_unique<GraphicsConfig>(); };

    Gleam_AddStartEvent(InitGraphicsLibrary, InitWindowLibraryOrder+1)
    {
        std::vector<const char*> extensions;
        Graphics::InitializeGLDemand(extensions);
        GL gl = GL::Initialize(Window::GetGlfwWindow(), extensions);
        Graphics::Initialize(gl, CreateGraphicsConfig());
    }
    Gleam_AddStopEvent(UnInitGraphicsLibrary, UnInitWindowLibraryOrder-1)
    {
        Graphics::UnInitialize();
        GL::UnInitialize();
    }

    Gleam_AddSystems(
        PresentationSystem
    )

#ifdef GleamEngineEditor
    Gleam_AddEditorSystems(
        PresentationSystem
    );
#endif
}