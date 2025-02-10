#include "Material.h"

namespace Gleam
{
    int Material::GetRenderQueue() const
    {
        return renderQueue;
    }
    void Material::SetRenderQueue(const int renderQueue)
    {
        this->renderQueue = renderQueue;
    }
}