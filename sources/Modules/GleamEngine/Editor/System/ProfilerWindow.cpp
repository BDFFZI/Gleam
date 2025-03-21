#include "ProfilerWindow.h"

#include "GleamUI/Runtime/UI.h"

#include "GleamEngine/Editor/Profiler.h"

namespace Gleam
{
    void SortProfile(Profile& profile)
    {
        auto children = profile.children | std::views::take(profile.childrenCount);
        std::ranges::sort(children, [](Profile& a, Profile& b)
        {
            return a.time > b.time;
        });
        for (auto& childProfile : children)
            SortProfile(childProfile);
    }
    void DrawProfile(Profile& profile)
    {
        ImGui::TableNextRow();

        //名称
        ImGui::TableNextColumn();
        bool drawChildren = profile.childrenCount != 0
                                ? ImGui::TreeNode(profile.name.data())
                                : (ImGui::BulletText(profile.name.data()), false);

        //总时间
        ImGui::TableNextColumn();
        ImGui::Text("%5.2f ms", profile.time);

        //自身时间
        ImGui::TableNextColumn();
        float selfTime = profile.time;
        for (int i = 0; i < profile.childrenCount; i++)
            selfTime -= profile.children[i].time;
        ImGui::Text("%5.2f ms", selfTime);

        //子运行数据
        if (drawChildren)
        {
            for (int i = 0; i < profile.childrenCount; i++)
                DrawProfile(profile.children[i]);

            ImGui::TreePop();
        }
    }
    void DrawRootProfile(const std::string_view tableName, Profile& profile)
    {
        if (ImGui::BeginTable(
            tableName.data(), 3,
            ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_Reorderable,
            float2(0.0f, ImGui::GetContentRegionAvail().y - ImGui::GetTextLineHeightWithSpacing())
        ))
        {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Total");
            ImGui::TableSetupColumn("Self");
            ImGui::TableHeadersRow();

            for (int i = 0; i < profile.childrenCount; i++)
                DrawProfile(profile.children[i]);

            ImGui::EndTable();
        }
    }

    void ProfilerWindow::SetProfile(Profile&& profiler)
    {
        lastProfile = std::move(profiler);
    }
    void ProfilerWindow::SetShowProfile(Profile& profile)
    {
        showProfile = profile;
        if (sort)
            SortProfile(showProfile);
    }
    void ProfilerWindow::Update()
    {
        ImGui::Begin("ProfilerWindow");

        if (ImGui::CollapsingHeader("LastProfile"))
            DrawRootProfile("LastProfile", lastProfile);

        if (ImGui::Button("Tick"))
        {
            tickProfile = lastProfile;
            SetShowProfile(tickProfile);
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("Sort", &sort))
        {
            SetShowProfile(tickProfile);
        }

        //显示探针
        ImGui::SameLine();
        if (ImGui::CollapsingHeader("TickProfile", ImGuiTreeNodeFlags_DefaultOpen))
            DrawRootProfile("TickProfile", showProfile);

        ImGui::End();
    }

    void ProfilerWindow_FetchProfile()
    {
        GlobalProfilerWindow.SetProfile(Profiler::FlushProfile());
    }
}