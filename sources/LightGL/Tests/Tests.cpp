#include <chrono>

#include "LightGL/Runtime/GL.h"
#include "LightGL/Runtime/Pipeline/GLSwapChain.h"
#include "LightGL/Runtime/Resource/GLImageView.h"

#include "LightImport/Runtime/ImageImporter.h"
#include "LightImport/Runtime/ModelImporter.h"
#include "LightImport/Runtime/ShaderImporter.h"
#include "LightMath/Runtime/Matrix.hpp"
#include "LightMath/Runtime/MatrixMath.hpp"
#include "LightUtility/Runtime/Utility.hpp"

using namespace Light;

struct Vertex
{
    float3 pos;
    float3 color;
    float2 texCoord;
};

struct ConstantBuffer
{
    ///vulkan对常量缓冲区内存布局要求必须16字节对齐
    ///可用直接用float2（16byte），float4（32byte）等，但float3就不符合（24byte）
    alignas(16) float4x4 model;
    alignas(16) float4x4 view;
    alignas(16) float4x4 proj;
};

class GLTester
{
    std::unique_ptr<GLSwapChain> glSwapChain;
    size_t maxFramesInFlight;
    std::unique_ptr<GLImage> colorImage;
    std::unique_ptr<GLImageView> colorImageView;
    std::unique_ptr<GLImage> depthImage;
    std::unique_ptr<GLImageView> depthImageView;
    std::vector<std::unique_ptr<GLFramebuffer>> glFramebuffers;

    std::unique_ptr<GLRenderPass> glRenderPass;
    // descriptorSetLayout;
    std::unique_ptr<GLPipelineLayout> glPipelineLayout;
    std::unique_ptr<GLPipeline> glPipeline;

    std::unique_ptr<GLBuffer> vertexBuffer;
    std::unique_ptr<GLBuffer> indexBuffer;
    std::unique_ptr<GLBuffer> uniformBuffer;
    std::unique_ptr<GLImage> texture;
    std::unique_ptr<GLImageView> textureView;
    std::unique_ptr<GLImageSampler> textureSampler;
    std::unique_ptr<GLDescriptorPool> glDescriptorPool;
    std::vector<std::unique_ptr<GLDescriptorSet>> glDescriptorSets;

    std::vector<std::unique_ptr<GLCommandBuffer>> glCommandBuffers;

public:
    GLTester()
    {
        //创建交换链
        RecreateSwapChain();

        //创建渲染pass
        std::vector<GLAttachmentInfo> glAttachments = {
            {GLAttachmentType::Color, glSwapChain->imageFormat, GL::glDevice->maxUsableSampleCount},
            {GLAttachmentType::DepthStencil, depthImageView->format, GL::glDevice->maxUsableSampleCount},
            {GLAttachmentType::ColorResolve, glSwapChain->imageFormat, VK_SAMPLE_COUNT_1_BIT}
        };
        std::vector<GLSubpass> glSubpasses = {
            {
                {VkAttachmentReference{0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}},
                VkAttachmentReference{1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL},
                VkAttachmentReference{2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
            }
        };

        //subPass处理pass过程中的图像布局变换，而pass的开始和结束时也有两个隐式“subPass”处理对应变换（上文设置的那些）。
        //由于使用的异步渲染，多个图形命令将被执行，也因此会存在冲突（比如帧缓冲区的图像正被上次命令中交换链读取，而此刻又有新的命令需要对其写入）。
        //因此需要Dependency功能指出subpass运行的依赖信息，这样GPU同时执行多道命令时便会对之间的执行时机进行一定控制。
        VkSubpassDependency dependency{};
        //通过同时指定src和dst来确定subpass的执行时机（执行时机前后相邻的两个subpass）
        //dst必须高于src以避免循环，但序号可以不连续，这意味着可以有多个subpass在同样的起点并行执行
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL; //在srcSubPass中定义该值表示前置隐式subpass（放dstSubpass中表示后置）
        dependency.dstSubpass = 0; //表明srcSubpass的后一个执行的subpass，此处即我们的subpass，通过srcSubpass+dstSubpass便实现了subpass执行顺序的确定
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT; //等待交换链完成从图像的读取（颜色附件可用）并且上一次片段测试结束时执行（深度附件可用）
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT; //等待交换链完成从图像的读取（颜色附件可用）并且当前片段测试开始（使用深度附件）
        dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        //当颜色和深度缓冲区可写入时执行（多缓冲帧共用了一样的附件）
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        //当颜色和深度缓冲区可写入时执行
        std::vector glSubpassDependencies = {dependency};

        glRenderPass = std::make_unique<GLRenderPass>(
            glAttachments, glSubpasses, glSubpassDependencies
        );

        //创建帧缓冲区
        RecreateFrameBuffers();

        //网格布局
        GLMeshLayout glMeshLayout{
            GLVertexInput(
                sizeof(Vertex), std::vector<GLVertexAttribute>{
                    {0,offsetof(Vertex, pos), VK_FORMAT_R32G32B32_SFLOAT},
                    {1,offsetof(Vertex, color), VK_FORMAT_R32G32B32_SFLOAT},
                    {2,offsetof(Vertex, texCoord), VK_FORMAT_R32G32_SFLOAT},
                }
            ),
            GLInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false)
        };
        //管线布局
        std::unique_ptr<GLDescriptorSetLayout> descriptorSetLayout = std::make_unique<GLDescriptorSetLayout>(
            std::vector{
                GLDescriptorBinding{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
                GLDescriptorBinding{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
            });
        glPipelineLayout = std::make_unique<GLPipelineLayout>(*descriptorSetLayout, std::vector<VkPushConstantRange>{});
        //着色器布局
        std::string code = Utility::ReadFile("Assets/shader.hlsl");
        std::vector glShaderLayout{
            GLShader(VK_SHADER_STAGE_VERTEX_BIT,
                     ShaderImporter::ImportHlsl(code, shaderc_vertex_shader, "VertexShader"),
                     "VertexShader"),
            GLShader(VK_SHADER_STAGE_FRAGMENT_BIT,
                     ShaderImporter::ImportHlsl(code, shaderc_fragment_shader, "FragmentShader"),
                     "FragmentShader"),
        };
        //创建渲染管线
        GLStateLayout stateLayout = {};
        stateLayout.SetRasterizationSamples(GL::glDevice->maxUsableSampleCount);
        stateLayout.SetViewportCount(1, 1);
        stateLayout.dynamicStates.insert(
            stateLayout.dynamicStates.end(), {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR,
            });
        glPipeline = std::make_unique<GLPipeline>(
            *glRenderPass, 0,
            glShaderLayout, glMeshLayout, *glPipelineLayout,
            stateLayout
        );

        //创建顶点索引缓冲区
        RawMesh mesh = ModelImporter::ImportObj("Assets/viking_room.obj");
        std::vector<Vertex> vertices(mesh.positions.size());
        for (size_t i = 0; i < vertices.size(); ++i)
        {
            vertices[i] = {
                mesh.positions[i],
                {0.1f, 1, 1},
                {mesh.uvs[i].x, 1 - mesh.uvs[i].y} //由于图片加载像素顺序的问题，vulkan会误认为左上角为原点，故需要反转y轴
            };
        }

        vertexBuffer = std::make_unique<GLBuffer>(
            static_cast<int>(sizeof(Vertex) * vertices.size()),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            vertices.data()
        );
        indexBuffer = std::make_unique<GLBuffer>(
            static_cast<int>(sizeof(uint32_t) * mesh.triangles.size()),
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            mesh.triangles.data()
        );

        //创建常量缓冲区（不用推送常量是因为：推送常量有大小限制，在有些显卡上只有128字节，但统一缓冲区（192字节）已经超过）
        uniformBuffer = GLBuffer::CreateUniformBuffer(sizeof(ConstantBuffer));

        //创建纹理及采样器
        RawImage rawImage = ImageImporter::Import("Assets/viking_room.png", STBI_rgb_alpha);
        texture = GLImage::CreateTexture2D(
            rawImage.width, rawImage.height, VK_FORMAT_R8G8B8A8_SRGB,
            rawImage.pixels.data(), rawImage.pixels.size(), true);
        textureView = std::make_unique<GLImageView>(*texture, VK_IMAGE_ASPECT_COLOR_BIT);
        textureSampler = std::make_unique<GLImageSampler>();

        //创建描述符集并绑定资源
        glDescriptorPool = std::make_unique<
            GLDescriptorPool>(*descriptorSetLayout, static_cast<int>(maxFramesInFlight));
        glDescriptorSets.resize(maxFramesInFlight);
        for (size_t i = 0; i < maxFramesInFlight; ++i)
        {
            glDescriptorSets[i] = std::make_unique<GLDescriptorSet>(*glDescriptorPool, *descriptorSetLayout);
            glDescriptorSets[i]->BindBuffer(0, *uniformBuffer);
            glDescriptorSets[i]->BindImage(1, *textureView, *textureSampler);
        }

        //创建命令缓冲区
        glCommandBuffers.resize(maxFramesInFlight);
        for (size_t i = 0; i < maxFramesInFlight; ++i)
            glCommandBuffers[i] = std::make_unique<GLCommandBuffer>();
    }

    ~GLTester()
    {
        vkDeviceWaitIdle(GL::glDevice->device);
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
            vkDeviceWaitIdle(GL::glDevice->device);
            glFramebuffers.clear();
            glSwapChain.reset();
            RecreateSwapChain();
            RecreateFrameBuffers();
            return;
        }

        //计算统一缓冲区
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float>(currentTime - startTime).count();
        ConstantBuffer ubo;
        ubo.model = float4x4::TRS(0, {-90, time * 90, 0}, 1);
        ubo.view = inverse(float4x4::TRS({2, 2, 2}, {32, -135, 0}, 1));
        ubo.proj = float4x4::Perspective(45.0f, static_cast<float>(glSwapChain->imageExtent.width) / static_cast<float>(glSwapChain->imageExtent.height), 0.1f, 10.0f);
        ubo.proj._m11 *= -1; //float4x4以Direct3D为准，输出的剪辑空间坐标y与vk相反，故需反转。
        void* bufferMemory = uniformBuffer->MapMemory();
        memcpy(bufferMemory, &ubo, sizeof(ConstantBuffer));
        uniformBuffer->UnmapMemory();

        //准备绘图命令
        GLCommandBuffer& glCommandBuffer = *glCommandBuffers[bufferIndex];
        glCommandBuffer.BeginRecording();
        glCommandBuffer.SetViewport(0, 0, static_cast<float>(glSwapChain->imageExtent.width), static_cast<float>(glSwapChain->imageExtent.height));
        glCommandBuffer.SetScissor({0, 0}, glSwapChain->imageExtent);
        glCommandBuffer.BeginRenderPass(*glRenderPass, *glFramebuffers[imageIndex]);
        glCommandBuffer.BindPipeline(*glPipeline);
        glCommandBuffer.BindVertexBuffers(*vertexBuffer);
        glCommandBuffer.BindIndexBuffer(*indexBuffer);
        glCommandBuffer.BindDescriptorSet(*glPipelineLayout, *glDescriptorSets[bufferIndex]);
        glCommandBuffer.DrawIndexed(static_cast<int>(indexBuffer->size / sizeof(uint32_t)));
        glCommandBuffer.EndRenderPass();
        glCommandBuffer.EndRecording();

        //提交命令
        glCommandBuffer.SubmitCommandsAsync(
            {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}, //在颜色输出阶段要进行等待
            {imageAvailable}, //等待到交换链的下一张图片可用时继续
            {renderFinishedSemaphores} //完成后发出渲染完成信号量
        );

        glSwapChain->PresentImageAsync();
    }

    void WaitDrawFrame() const
    {
        //等待信号量可用
        glCommandBuffers[glSwapChain->GetCurrentBufferIndex()]->WaitSubmissionFinish();
    }

    void RecreateSwapChain()
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(GL::glSurface->window, &width, &height);
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(GL::glSurface->window, &width, &height);
            glfwWaitEvents();
        }

        glSwapChain = std::make_unique<GLSwapChain>();
        maxFramesInFlight = glSwapChain->images.size();

        colorImage = GLImage::CreateFrameBufferColor(
            glSwapChain->imageExtent.width, glSwapChain->imageExtent.height,
            glSwapChain->imageFormat, GL::glDevice->maxUsableSampleCount);
        colorImageView = std::make_unique<GLImageView>(*colorImage, VK_IMAGE_ASPECT_COLOR_BIT);

        VkFormat depthFormat = GL::glDevice->FindImageFormat(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );

        depthImage = GLImage::CreateFrameBufferDepth(
            glSwapChain->imageExtent.width, glSwapChain->imageExtent.height,
            depthFormat, GL::glDevice->maxUsableSampleCount);
        depthImageView = std::make_unique<GLImageView>(*depthImage, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    void RecreateFrameBuffers()
    {
        //创建帧缓冲区
        glFramebuffers.resize(maxFramesInFlight);
        for (size_t i = 0; i < maxFramesInFlight; ++i)
        {
            glFramebuffers[i] = std::make_unique<GLFramebuffer>(
                *glRenderPass,
                std::vector{
                    colorImageView->imageView,
                    depthImageView->imageView,
                    glSwapChain->imageViews[i]->imageView
                },
                glSwapChain->imageExtent);
        }
    }
};

// TEST(GLTests, ALL)
void main()
{
    constexpr uint32_t WIDTH = 800;
    constexpr uint32_t HEIGHT = 600;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    GL::Initialize(window);
    {
        GLTester glTester = {};
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();

            //逻辑处理

            glTester.WaitDrawFrame();
            glTester.DrawFrame();
        }
    }
    GL::UnInitialize();
}
