#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

struct GLShader
{
    std::vector<char> shaderCode;
    VkShaderStageFlagBits shaderStage;
    std::string shaderName;

    /**
     * 
     * @param codeFilePath shader字节码的文件地址
     * @param shaderStage shader对应的图像管道阶段
     * @param shaderName shader在文件源码中的函数名
     * @return 
     */
    GLShader(const std::string& codeFilePath, VkShaderStageFlagBits shaderStage, const std::string& shaderName);
};