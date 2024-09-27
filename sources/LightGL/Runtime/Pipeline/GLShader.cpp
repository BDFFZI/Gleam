#include "GLShader.h"

#include <fstream>
#include <vector>

#include "../GL.h"

GLShader::GLShader(const std::vector<char>& shaderCode, std::string shaderName, const VkShaderStageFlagBits shaderStage)
    : shaderCode(shaderCode), shaderName(std::move(shaderName)), shaderStage(shaderStage)
{
}
