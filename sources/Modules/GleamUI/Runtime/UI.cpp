#include "UI.h"

#include <filesystem>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "GleamGraphics/Runtime/Graphics.h"
#include "GleamGraphics/Runtime/SwapChain.h"
#include "GleamWindow/Runtime/System/InputSystem.h"
#include "GleamPresentation/Runtime/PresentationSystem.h"
#include "GleamUtility/Runtime/String.h"
#include "GleamWindow/Runtime/Window.h"

namespace Gleam
{
    void CheckVKResult(const VkResult err)
    {
        if (err >= 0)
            return;
        abort();
    }

    void UI::Init()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // 启用ImGui键盘按钮功能
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; //启用ImGui船坞功能
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange; //禁止ImGui修改光标可见性

        //优先使用StreamingAssets中提供的布局文件，这些可能是用户有意提供的
        if (std::filesystem::exists("StreamingAssets/imgui.ini") && !std::filesystem::exists("imgui.ini"))
            std::filesystem::copy("StreamingAssets/imgui.ini", "imgui.ini");

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
    }
    void UI::UnInit()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        descriptorPool.reset();
        descriptorSetLayout.reset();
    }
    void UI::BeginFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
    void UI::EndFrame(GCommandBuffer& commandBuffer)
    {
        //生成绘制数据
        ImGui::Render();
        //提交绘制命令
        commandBuffer.SetRenderTarget(SwapChain::GetPresentRenderTarget());
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer.GetGLCommandBuffer().commandBuffer);
    }

    ImTextureID UI::CreateTexture(GTexture& texture)
    {
        const VkDescriptorSet descriptorSet = ImGui_ImplVulkan_AddTexture(
            texture.GetGLImageSampler().imageSampler,
            texture.GetGLImageView().imageView,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        return descriptorSet;
    }
    void UI::DeleteTexture(ImTextureID& texture)
    {
        ImGui_ImplVulkan_RemoveTexture(static_cast<VkDescriptorSet>(texture));
        texture = nullptr;
    }
    float2 UI::GetWindowContentRegionSize()
    {
        const float2 windowMin = ImGui::GetWindowContentRegionMin();
        const float2 windowMax = ImGui::GetWindowContentRegionMax();
        return windowMax - windowMin;
    }
    bool UI::DragScalarMatrix(const char* label, const ImGuiDataType data_type, void* p_data, const int width, const int height, const float v_speed)
    {
        bool isModified = false;

        if (ImGui::TreeNode(label)) //使用树部件显示矩阵名称并提供隐藏功能
        {
            if (ImGui::BeginTable("", width)) //使用表格布局按行列绘制矩阵
            {
                for (int row = 0; row < height; ++row)
                {
                    ImGui::TableNextRow();
                    for (int col = 0; col < width; ++col)
                    {
                        ImGui::TableNextColumn();

                        int index = col * height + row;
                        std::byte* address = &static_cast<std::byte*>(p_data)[index * ImGui::DataTypeGetInfo(data_type)->Size];

                        ImGui::PushItemWidth(-FLT_MIN); //隐藏标签显示
                        isModified |= ImGui::DragScalar(std::format("##{}{}", row, col).c_str(), data_type, address, v_speed);
                        ImGui::PopItemWidth();
                    }
                }

                ImGui::EndTable();
            }

            ImGui::TreePop();
        }

        return isModified;
    }

    void UI::MenuItem(const std::vector<std::string>& path, const std::function<void()>& func, const size_t layer)
    {
        if (layer > path.size())
            return;

        if (layer == path.size() - 1)
        {
            if (ImGui::MenuItem(path.back().c_str()))
            {
                func();
            }
        }
        else
        {
            if (ImGui::BeginMenu(path[layer].c_str()))
            {
                MenuItem(path, func, layer + 1);
                ImGui::EndMenu();
            }
        }
    }
    void UI::Menu(const std::unordered_map<std::string, std::function<void()>>& menuItems)
    {
        static std::vector<std::string> path = {};

        for (auto& [name,func] : menuItems)
        {
            String::Split(name, "/", path);
            MenuItem(path, func);
        }
    }
}