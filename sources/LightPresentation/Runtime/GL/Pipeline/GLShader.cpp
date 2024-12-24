#include "GLShader.h"

#include <fstream>
#include <vector>

#include "../GL.h"

GLShader::GLShader(
    const VkShaderStageFlagBits shaderStage,
    std::vector<std::byte> shaderBytecode,
    std::string shaderEntrance)
    : shaderStage(shaderStage), shaderBytecode(std::move(shaderBytecode)), shaderEntrance(std::move(shaderEntrance))
{
}
