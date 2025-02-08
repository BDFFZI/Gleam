﻿#include "UISystem.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "LightGL/Runtime/GL.h"
#include "LightGraphics/Runtime/SwapChain.h"
#include "LightWindow/Runtime/Window.h"

namespace Light
{
    void CheckVKResult(const VkResult err)
    {
        if (err >= 0)
            return;
        abort();
    }

    void UISystem::Start()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // 启用ImGui键盘按钮功能
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; //启用ImGui船坞功能
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange; //禁止ImGui修改光标可见性

        descriptorSetLayout = std::make_unique<GLDescriptorSetLayout>(std::vector<GLDescriptorBinding>{
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
        });
        descriptorPool = std::make_unique<GLDescriptorPool>(*descriptorSetLayout, 10, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);

        VkPipelineRenderingCreateInfo renderingInfo = {};
        renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        renderingInfo.pNext = VK_NULL_HANDLE;
        VkFormat colorFormat[] = {Graphics::GetGraphicsConfig().presentColorFormat};
        renderingInfo.colorAttachmentCount = std::size(colorFormat);
        renderingInfo.pColorAttachmentFormats = colorFormat;
        renderingInfo.depthAttachmentFormat = Graphics::GetGraphicsConfig().presentDepthStencilFormat;
        renderingInfo.stencilAttachmentFormat = Graphics::GetGraphicsConfig().presentDepthStencilFormat;

        // Setup Platform/Renderer backends

        ImGui_ImplGlfw_InitForVulkan(Window::GetGlfwWindow(), true);
        ImGui_ImplVulkan_InitInfo initInfo = {};
        initInfo.Instance = GL::glInstance->instance;
        initInfo.PhysicalDevice = GL::glDevice->physicalDevice;
        initInfo.Device = GL::glDevice->device;
        initInfo.QueueFamily = GL::glDevice->graphicQueueFamily;
        initInfo.Queue = GL::glDevice->graphicQueue;
        initInfo.DescriptorPool = descriptorPool->descriptorPool; // See requirements in note above
        initInfo.MinImageCount = SwapChain::GetGLSwapChain().minImageCount; // >= 2
        initInfo.ImageCount = SwapChain::GetGLSwapChain().imageCount; // >= MinImageCount
        initInfo.MSAASamples = Graphics::GetGraphicsConfig().presentSampleCount; // 0 defaults to VK_SAMPLE_COUNT_1_BIT
        initInfo.UseDynamicRendering = true;
        initInfo.PipelineRenderingCreateInfo = renderingInfo;
        initInfo.CheckVkResultFn = CheckVKResult;
        ImGui_ImplVulkan_Init(&initInfo);
        ImGui_ImplVulkan_CreateFontsTexture();

        SystemGroup::Start();
    }
    void UISystem::Stop()
    {
        SystemGroup::Stop();

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        descriptorPool.reset();
        descriptorSetLayout.reset();
    }
    void UISystem::Update()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        SystemGroup::Update();

        //生成绘制数据
        ImGui::Render();
        //提交绘制命令
        GCommandBuffer& commandBuffer = PresentationSystem->GetPresentGCommandBuffer();
        commandBuffer.SetRenderTarget(SwapChain::GetPresentRenderTarget());
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer.GetGLCommandBuffer().commandBuffer);
    }
}