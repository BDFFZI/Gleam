// #pragma once
// #include <memory>
//
// #include "../GL/Pipeline/GLPipeline.h"
// #include "../GL/Resource/GLDescriptorPool.h"
// #include "../GL/Resource/GLDescriptorSet.h"
//
// struct PassInfo
// {
//     std::vector<GLShader> shaderLayout;
//     GLMeshLayout meshLayout;
//     std::vector<GLDescriptorBinding> descriptorLayout;
// };
//
// class Pass
// {
// public:
//     {
//         =
//
//         //创建渲染管线
//         GLMeshLayout glMeshLayout(sizeof(Vertex), {
//                                       GLVertexAttribute{offsetof(Vertex, pos), VK_FORMAT_R32G32B32_SFLOAT},
//                                       GLVertexAttribute{offsetof(Vertex, color), VK_FORMAT_R32G32B32_SFLOAT},
//                                       GLVertexAttribute{offsetof(Vertex, texCoord), VK_FORMAT_R32G32_SFLOAT},
//                                   }, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST); //网格布局
//
//         std::unique_ptr<GLDescriptorSetLayout> descriptorSetLayout = std::make_unique<GLDescriptorSetLayout>(std::vector{
//             GLDescriptorBinding{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
//             GLDescriptorBinding{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
//         });
//         glPipelineLayout = std::make_unique<GLPipelineLayout>(*descriptorSetLayout); //描述符布局
//         std::vector glShaderLayout{
//             GLShader("shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main"),
//             GLShader("shaders/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main"),
//         }; //着色器
//         glPipeline = std::make_unique<GLPipeline>(*glRenderPass, glShaderLayout, glMeshLayout, *glPipelineLayout);
//     }
// };
//
//
// class Shader
// {
//     struct Pass
//     {
//         GLDescriptorSetLayout descriptorSetLayout;
//         GLDescriptorPool descriptorPool;
//         GLDescriptorSet descriptorSet;
//         GLPipelineLayout pipelineLayout;
//         GLPipeline pipeline;
//
//         Pass(const PassInfo& passInfo, const GLRenderPass& renderPass, const int passIndex)
//             : descriptorSetLayout(passInfo.descriptorLayout),
//               descriptorPool(descriptorSetLayout, 1),
//               descriptorSet(descriptorPool, descriptorSetLayout),
//               pipelineLayout(descriptorSetLayout),
//               pipeline(renderPass, passIndex, passInfo.shaderLayout, passInfo.meshLayout, pipelineLayout)
//         {
//         }
//     };
//
// public:
//     Shader(const std::vector<PassInfo>& passInfos, const VkFormat& outputColorFormat, const VkFormat& outputDepthFormat)
//         : renderPass(outputColorFormat, outputDepthFormat)
//     {
//         for (size_t i = 0; i < passInfos.size(); ++i)
//             passes.push_back(std::make_unique<Pass>(passInfos[i], renderPass, i));
//     }
//
//     GLRenderPass renderPass;
//     std::vector<std::unique_ptr<Pass>> passes;
// };
