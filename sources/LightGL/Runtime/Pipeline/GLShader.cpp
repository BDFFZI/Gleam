#include "GLShader.h"

#include <fstream>
#include <vector>

#include "../GL.h"

GLShader::GLShader(const VkShaderStageFlagBits shaderStage, const std::vector<std::byte>& shaderCode, std::string entryPoint)
    : shaderCode(shaderCode), entryPoint(std::move(entryPoint)), shaderStage(shaderStage)
{
}
