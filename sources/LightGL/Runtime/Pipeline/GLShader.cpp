#include "GLShader.h"

#include <fstream>
#include <vector>

#include "../Foundation/GLFoundation.h"

std::vector<char> ReadFile(const std::string& filename)
{
    //通过ate标志初始就将指针放在流末尾
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("文件打开失败！");

    //由于指针在流末尾，故其位置即文件长度
    const std::streamsize fileSize = file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

GLShader::GLShader(const std::string& codeFilePath, const VkShaderStageFlagBits shaderStage, const std::string& shaderName)
    : shaderStage(shaderStage), shaderName(shaderName)
{
    shaderCode = ReadFile(codeFilePath);
}