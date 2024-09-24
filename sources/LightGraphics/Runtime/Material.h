#pragma once
#include "Shader.h"
#include "LightGL/Runtime/Resource/GLDescriptorSet.h"

class MaterialBase
{
public:
    MaterialBase(const std::shared_ptr<Shader>& shader): shader(shader)
    {
    }
    virtual ~MaterialBase() = default;

    virtual const Shader& GetShader() = 0;
private:
    std::shared_ptr<Shader> shader;
};

class Material
{
};
