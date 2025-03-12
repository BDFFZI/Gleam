#include "CustomUI.h"

#include "GleamRendering/Editor/Handles.h"

namespace Gleam
{
    void SceneWindowUI_Particle(Particle& particle)
    {
        particle.position = Handles::DrawHandle(particle.position);
    }
}