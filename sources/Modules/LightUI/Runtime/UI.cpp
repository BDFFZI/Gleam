#include "UI.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "LightWindow/Runtime/Input.h"

namespace Light
{
    ImTextureID UI::CreateTexture(GTexture& texture)
    {
        const VkDescriptorSet descriptorSet = ImGui_ImplVulkan_AddTexture(
            texture.GetGLImageSampler().imageSampler,
            texture.GetGLImageView().imageView,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        return descriptorSet;
    }
    void UI::DeleteTexture(const ImTextureID texture)
    {
        ImGui_ImplVulkan_RemoveTexture(static_cast<VkDescriptorSet>(texture));
    }
    float2 UI::GetWindowContentRegionSize()
    {
        const float2 windowMin = ImGui::GetWindowContentRegionMin();
        const float2 windowMax = ImGui::GetWindowContentRegionMax();
        return windowMax - windowMin;
    }
}