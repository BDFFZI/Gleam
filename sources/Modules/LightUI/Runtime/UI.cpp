#include "UI.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "LightWindow/Runtime/Input.h"
#include "LightMath/Runtime/LinearAlgebra/MatrixMath.h"

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
    bool UI::DragFloat4x4(const char* label, float4x4* v, const float v_speed)
    {
        bool4x4 result = false;
        
        if (ImGui::TreeNode(label)) //使用树部件显示矩阵名称并提供隐藏功能
        {
            if (ImGui::BeginTable("", 4)) //使用表格布局按行列绘制矩阵
            {
                for (int row = 0; row < 4; ++row)
                {
                    ImGui::TableNextRow();
                    for (int col = 0; col < 4; ++col)
                    {
                        ImGui::TableNextColumn();

                        ImGui::PushItemWidth(-FLT_MIN); //隐藏标签显示
                        result[row][col] = ImGui::DragFloat(std::format("##{}{}", row, col).c_str(), &(*v)[row][col], v_speed);
                        ImGui::PopItemWidth();
                    }
                }

                ImGui::EndTable();
            }

            ImGui::TreePop();
        }

        return any(result);
    }
}