#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

struct GLShader
{
    VkShaderStageFlagBits shaderStage;
    std::vector<std::byte> shaderBytecode;
    std::string shaderEntrance;

    /**
     * 
     * @param shaderStage shader作用的图像管道阶段
     * @param shaderBytecode shader编译后的字节码
     * @param shaderEntrance shader源码中的函数名
     * @return 
     */
    GLShader(VkShaderStageFlagBits shaderStage, std::vector<std::byte> shaderBytecode, std::string shaderEntrance);
};

class GLShaderLayout
{
public:
    std::vector<GLShader> shaders;
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

    explicit GLShaderLayout(const std::vector<GLShader>& shaders);
    ~GLShaderLayout();
};