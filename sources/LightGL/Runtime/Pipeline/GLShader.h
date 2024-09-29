#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

struct GLShader
{
    VkShaderStageFlagBits shaderStage;
    std::vector<std::byte> shaderCode;
    std::string entryPoint;

    /**
     * 
     * @param shaderStage shader作用的图像管道阶段
     * @param shaderCode shader编译后的字节码
     * @param entryPoint shader源码中的函数名
     * @return 
     */
    GLShader(VkShaderStageFlagBits shaderStage, const std::vector<std::byte>& shaderCode, std::string entryPoint);
};
