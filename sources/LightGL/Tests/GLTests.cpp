#include <chrono>
#include <vulkan/vulkan_core.h>
#include <functional>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <gtest/gtest.h>

#include <LightGL/Runtime/GL.h>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    bool operator==(const Vertex& other) const
    {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};

template <>
struct std::hash<Vertex>
{
    size_t operator()(Vertex const& vertex) const noexcept
    {
        return ((hash<glm::vec3>()(vertex.pos) ^
                (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
            (hash<glm::vec2>()(vertex.texCoord) << 1);
    }
};

inline void LoadModel(const std::string& modelPath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    static std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str()))
        throw std::runtime_error(warn + err);

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex{};
            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = {1.0f, 1.0f, 1.0f};

            if (!uniqueVertices.contains(vertex))
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }
}

struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

class GLTester
{
    std::unique_ptr<GLSwapChain> glSwapChain;
    size_t maxFramesInFlight;
    std::unique_ptr<GLImage> depthImage;
    std::unique_ptr<GLImageView> depthImageView;
    std::vector<std::unique_ptr<GLFramebuffer>> glFramebuffers;

    std::unique_ptr<GLRenderPass> glRenderPass;
    // descriptorSetLayout;
    std::unique_ptr<GLPipelineLayout> glPipelineLayout;
    std::unique_ptr<GLPipeline> glPipeline;

    std::unique_ptr<GLBuffer> vertexBuffer;
    std::unique_ptr<GLBuffer> indexBuffer;
    std::vector<std::unique_ptr<GLBuffer>> constantBuffers;
    std::vector<void*> constantBufferAddresses;
    std::unique_ptr<GLImage> texture;
    std::unique_ptr<GLImageView> textureView;
    std::unique_ptr<GLImageSampler> textureSampler;
    std::unique_ptr<GLDescriptorPool> glDescriptorPool;
    std::vector<std::unique_ptr<GLDescriptorSet>> glDescriptorSets;

    std::vector<std::unique_ptr<GLCommandBuffer>> glCommandBuffers;

public:
    GLTester(GLFWwindow* window)
    {
        GLFoundation::Initialize(window);

        //创建交换链
        RecreateSwapChain();

        //创建渲染pass
        glRenderPass = std::make_unique<GLRenderPass>(
            glSwapChain->swapChainImageViews[0]->format,
            depthImageView->format
        );

        //创建帧缓冲区
        RecreateFrameBuffers();

        //创建渲染管线
        GLMeshLayout glMeshLayout(sizeof(Vertex), {
                                      GLVertexAttribute{offsetof(Vertex, pos), VK_FORMAT_R32G32B32_SFLOAT},
                                      GLVertexAttribute{offsetof(Vertex, color), VK_FORMAT_R32G32B32_SFLOAT},
                                      GLVertexAttribute{offsetof(Vertex, texCoord), VK_FORMAT_R32G32_SFLOAT},
                                  }, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST); //网格布局

        std::unique_ptr<GLDescriptorSetLayout> descriptorSetLayout = std::make_unique<GLDescriptorSetLayout>(std::vector{
            GLDescriptorBinding{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
            GLDescriptorBinding{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
        });
        glPipelineLayout = std::make_unique<GLPipelineLayout>(*descriptorSetLayout); //描述符布局
        std::vector glShaderLayout{
            GLShader("assets/vert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main"),
            GLShader("assets/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main"),
        }; //着色器
        glPipeline = std::make_unique<GLPipeline>(*glRenderPass, 0, glShaderLayout, glMeshLayout, *glPipelineLayout);

        //创建顶点索引缓冲区
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        LoadModel("assets/viking_room.obj", vertices, indices);
        vertexBuffer = std::make_unique<GLBuffer>(
            static_cast<int>(sizeof(Vertex) * vertices.size()),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            vertices.data()
        );
        indexBuffer = std::make_unique<GLBuffer>(
            static_cast<int>(sizeof(uint32_t) * indices.size()),
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            indices.data()
        );

        //创建常量缓冲区
        constantBuffers.resize(maxFramesInFlight);
        constantBufferAddresses.resize(maxFramesInFlight);
        for (size_t i = 0; i < maxFramesInFlight; ++i)
        {
            constantBuffers[i] = std::make_unique<GLBuffer>(
                sizeof(UniformBufferObject),
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            constantBufferAddresses[i] = constantBuffers[i]->MapMemory();
        }

        //创建纹理及采样器
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load("assets/viking_room.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        const int imageSize = texWidth * texHeight * 4;
        if (!pixels)
            throw std::runtime_error("加载图片数据失败!");
        texture = std::unique_ptr<GLImage>(GLImage::CreateTexture2D(texWidth, texHeight, pixels, imageSize, true));
        textureView = std::make_unique<GLImageView>(*texture, VK_IMAGE_ASPECT_COLOR_BIT);
        textureSampler = std::make_unique<GLImageSampler>();

        //创建描述符集并绑定资源
        glDescriptorPool = std::make_unique<GLDescriptorPool>(*descriptorSetLayout, static_cast<int>(maxFramesInFlight));
        glDescriptorSets.resize(maxFramesInFlight);
        for (size_t i = 0; i < maxFramesInFlight; ++i)
        {
            glDescriptorSets[i] = std::make_unique<GLDescriptorSet>(*glDescriptorPool, *descriptorSetLayout);
            glDescriptorSets[i]->BindBuffer(0, *constantBuffers[i]);
            glDescriptorSets[i]->BindImage(1, *textureView, *textureSampler);
        }

        //创建命令缓冲区
        glCommandBuffers.resize(maxFramesInFlight);
        for (size_t i = 0; i < maxFramesInFlight; ++i)
            glCommandBuffers[i] = std::make_unique<GLCommandBuffer>();
    }


    void DrawFrame()
    {
        uint32_t imageIndex; //获取交换链下次呈现的图片索引
        uint32_t bufferIndex;
        VkSemaphore imageAvailable;
        VkSemaphore renderFinishedSemaphores;
        if (glSwapChain->SwitchImageAsync(&imageIndex, &bufferIndex,
                                          &imageAvailable, &renderFinishedSemaphores) == false)
        {
            vkDeviceWaitIdle(GLFoundation::glDevice->device);
            glFramebuffers.clear();
            glSwapChain.reset();
            RecreateSwapChain();
            RecreateFrameBuffers();
            return;
        }

        //更新常量缓冲区
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float>(currentTime - startTime).count();
        UniformBufferObject ubo;
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f),
                                    static_cast<float>(glSwapChain->swapChainImageExtent.width) / static_cast<float>(glSwapChain->swapChainImageExtent.height), 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;
        memcpy(constantBufferAddresses[bufferIndex], &ubo, sizeof(ubo));

        //准备绘图命令
        GLCommandBuffer& glCommandBuffer = *glCommandBuffers[bufferIndex];
        glCommandBuffer.BeginRecord();
        glCommandBuffer.BeginRenderPass(*glRenderPass, *glFramebuffers[imageIndex]);
        glCommandBuffer.BindPipeline(*glPipeline);
        glCommandBuffer.BindVertexBuffers(*vertexBuffer);
        glCommandBuffer.BindIndexBuffer(*indexBuffer);
        glCommandBuffer.BindDescriptorSets(*glPipelineLayout, *glDescriptorSets[bufferIndex]);
        glCommandBuffer.SetViewportAndScissor(glSwapChain->swapChainImageExtent);
        glCommandBuffer.Draw(static_cast<int>(indexBuffer->size / sizeof(uint32_t)));
        glCommandBuffer.EndRenderPass();
        glCommandBuffer.EndRecord();

        //提交命令
        glCommandBuffer.ExecuteCommandBufferAsync(
            {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}, //在颜色输出阶段要进行等待
            {imageAvailable}, //等待到交换链的下一张图片可用时继续
            {renderFinishedSemaphores} //完成后发出渲染完成信号量
        );

        glSwapChain->PresentImageAsync();
    }
    void WaitDrawFrame() const
    {
        //等待信号量可用
        glCommandBuffers[glSwapChain->GetCurrentBufferIndex()]->WaitExecutionFinish();
    }
    void RecreateSwapChain()
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(GLFoundation::glSurface->window, &width, &height);
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(GLFoundation::glSurface->window, &width, &height);
            glfwWaitEvents();
        }

        glSwapChain = std::make_unique<GLSwapChain>();
        maxFramesInFlight = glSwapChain->swapChainImages.size();

        depthImage = std::unique_ptr<GLImage>(GLImage::CreateRenderTargetDepth(glSwapChain->swapChainImageExtent.width, glSwapChain->swapChainImageExtent.height));
        depthImageView = std::make_unique<GLImageView>(*depthImage, VK_IMAGE_ASPECT_DEPTH_BIT);
    }
    void RecreateFrameBuffers()
    {
        //创建帧缓冲区
        glFramebuffers.resize(maxFramesInFlight);
        for (size_t i = 0; i < maxFramesInFlight; ++i)
            glFramebuffers[i] = std::make_unique<GLFramebuffer>(*glRenderPass, *glSwapChain->swapChainImageViews[i], *depthImageView, glSwapChain->swapChainImageExtent);
    }
    ~GLTester()
    {
        vkDeviceWaitIdle(GLFoundation::glDevice->device);
    }
};

TEST(GLTests, ALL)
{
    constexpr uint32_t WIDTH = 800;
    constexpr uint32_t HEIGHT = 600;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    GLTester glTester(window);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        //逻辑处理

        glTester.WaitDrawFrame();
        glTester.DrawFrame();
    }
}
