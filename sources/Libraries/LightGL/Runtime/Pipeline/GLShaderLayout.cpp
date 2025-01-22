#include "GLShaderLayout.h"

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

GLShaderLayout::GLShaderLayout(const std::vector<GLShader>& shaders)
    : shaders(shaders)
{
    shaderStages.resize(this->shaders.size());
    for (size_t i = 0; i < shaderStages.size(); ++i)
    {
        VkShaderModule shaderModule;
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = this->shaders[i].shaderBytecode.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(this->shaders[i].shaderBytecode.data());
        if (vkCreateShaderModule(GL::glDevice->device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
            throw std::runtime_error("创建着色器模型失败!");

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = this->shaders[i].shaderStage;
        vertShaderStageInfo.pName = this->shaders[i].shaderEntrance.c_str();
        vertShaderStageInfo.module = shaderModule;

        shaderStages[i] = vertShaderStageInfo;
    }
}
GLShaderLayout::~GLShaderLayout()
{
    //回收着色器的包装器内存
    for (auto& shaderStage : shaderStages)
        vkDestroyShaderModule(GL::glDevice->device, shaderStage.module, nullptr);
}