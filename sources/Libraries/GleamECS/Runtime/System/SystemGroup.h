#pragma once
#include <functional>
#include <optional>
#include <set>

#include "System.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Profiler.h"
#endif

namespace Gleam
{
    struct SystemPtrComparer
    {
        bool operator()(const System* left, const System* right) const
        {
            if (left->GetOrder() == right->GetOrder())
                return left < right; //确保顺序相同时依然有大小之分，从而避免不同系统实例被误认为相等
            return left->GetOrder() < right->GetOrder();
        }
    };

    class SystemGroup : public System
    {
    public:
        SystemGroup(const std::optional<std::reference_wrapper<SystemGroup>>& group, int minOrder = MinOrder, int maxOrder = MaxOrder, const std::string_view& name = "");
        SystemGroup(System& system, OrderRelation orderRelation, const std::string_view& name = "");
        explicit SystemGroup(SystemGroup& group);
        SystemGroup(SystemGroup&&) noexcept = default;
        SystemGroup& operator=(SystemGroup&&) = default;

        /**
         * 统计所有子系统，包括递归，但不包含正在卸载的子系统。
         * 也即是在用户调用AddSubSystem和RemoveSubSystem后逻辑上子系统集合。
         * 排除生命周期的影响，用该结果反向插入后可重建相同的系统组。
         * @return 
         */
        std::vector<std::reference_wrapper<System>> GetSubSystems();
        void AddSubSystem(System& system);
        void RemoveSubSystem(System& system);

        void Start() override;
        void Stop() override;
        void Update() override
        {
            for (System* system : subSystems)
            {
#ifdef GleamEngineEditor
                auto& name = system->GetName();
                Gleam_ProfilerSample(name);
#endif
                system->Update();
            }
        }

        void FlushStartQueue();
        void FlushStopQueue();

    private:
        friend class HierarchyWindow;

        std::set<System*, SystemPtrComparer> subSystems = {};
        /// 虽然子系统的添加删除是延迟的，但并不支持遍历时的结构化更改。
        /// 例如Start时addingSystems被占用，但用户依然可能执行AddSubSystem函数，于是就会导致遍历异常。
        /// 此处延迟触发的真实原因是因为插入系统和删除系统是无序的，但系统本身是有序的，
        /// 为了满足系统的顺序安排，只有先缓存再汇总后才可知正确的执行顺序。
        std::set<System*, SystemPtrComparer> addingSystems = {};
        std::set<System*, SystemPtrComparer> removingSystems = {};
    };
}