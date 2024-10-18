#include "GLShader.h"

#include <fstream>
#include <vector>

#include "../GL.h"

GLShader::GLShader(const VkShaderStageFlagBits shaderStage, const std::vector<std::byte>& shaderBytecode, std::string shaderEntrance)
    : shaderBytecode(shaderBytecode), shaderEntrance(std::move(shaderEntrance)), shaderStage(shaderStage)
{
}
