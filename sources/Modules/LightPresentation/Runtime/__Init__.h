#pragma once
#include "LightWindow/Runtime/__Init__.h"

#include "PresentationSystem.h"

#ifdef Light_Editor
#include "LightEngine/Editor/Editor.h"
#endif

namespace Light
{
    //通过修改该变量可以调整Presentation初始化Graphics库的方式，UI不需要，但Render需要
    inline static std::function CreateGraphicsConfig = [] { return std::make_unique<GraphicsConfig>(); };

    Light_AddStartEvent(InitGraphicsLibrary, InitWindowLibraryOrder+1)
    {
        std::vector<const char*> extensions;
        Graphics::InitializeGLDemand(extensions);
        GL gl = GL::Initialize(Window::GetGlfwWindow(), extensions);
        Graphics::Initialize(gl, CreateGraphicsConfig());
    }
    Light_AddStopEvent(UnInitGraphicsLibrary, UnInitWindowLibraryOrder-1)
    {
        Graphics::UnInitialize();
        GL::UnInitialize();
    }

    Light_AddSystems(
        PresentationSystem
    )

#ifdef Light_Editor
    Light_AddEditorSystems(
        PresentationSystem
    );
#endif
}