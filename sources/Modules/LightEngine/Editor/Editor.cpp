#include "Editor.h"
#include "LightEngine/Runtime/Engine.h"

namespace Light
{
    bool& Editor::IsPlaying()
    {
        return Engine::isUpdating;
    }
}