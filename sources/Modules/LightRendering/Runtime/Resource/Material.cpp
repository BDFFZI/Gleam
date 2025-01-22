#include "Material.h"

namespace Light
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