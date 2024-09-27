#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

struct GLShader
{
    std::vector<char> shaderCode;
    std::string shaderName;
    VkShaderStageFlagBits shaderStage;

    /**
     * 
     * @param shaderCode shader编译后的字节码
     * @param shaderName shader源码中的函数名
     * @param shaderStage shader作用的图像管道阶段
     * @return 
     */
    GLShader(const std::vector<char>& shaderCode, std::string shaderName, VkShaderStageFlagBits shaderStage);
};
