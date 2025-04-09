#pragma once
#include <functional>
#include <optional>
#include <set>
#include <map>

#include "System.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Profiler.h"
#endif

namespace Gleam
{
    template <class TGroup, int TMinOrder, int TMaxOrder>
    class SystemGroupBase : public SystemBase<TGroup, TMinOrder, TMaxOrder>
    {
    public:
        static std::multimap<int, SystemUpdate>& GetSubSystemUpdates()
        {
            static std::multimap<int, SystemUpdate> subSystemUpdates;
            return subSystemUpdates;
        }

        static void Update(const EntityAllocator& entities)
        {
        }
    };


    // template <class TGroup = void, auto...>
    // class System : public SystemBase<TGroup, SystemMinOrder, SystemMaxOrder>
    // {
    // };
    //
    // template <class TGroup, int TMinOrder, int TMaxOrder>
    // class System<TGroup, TMinOrder, TMaxOrder> : public SystemBase<TGroup, TMinOrder, TMaxOrder>
    // {
    // };
    //
    // template <class TParentSystem, int Order>
    // class System<TParentSystem, Order, Order> : public SystemBase<TParentSystem, Order, Order>
    // {
    // };
    //
    // template <class TBrotherSystem, SystemRelation Relation>
    //     requires static_cast<int64_t>(TBrotherSystem::MaxOrder) - static_cast<int64_t>(TBrotherSystem::MinOrder) >= 2
    // class System<TBrotherSystem, Relation> : public SystemBase<typename TBrotherSystem::Group,
    //                                                            Relation == SystemRelation::Before ? TBrotherSystem::MinOrder : TBrotherSystem::Order,
    //                                                            Relation == SystemRelation::Before ? TBrotherSystem::Order : TBrotherSystem::MaxOrder>
    // {
    // };


    /**
     * 一种支持子系统的系统，可以实现系统的分类和更新方式的控制
     */
    class SystemGroup : public System
    {
    public:
        SystemGroup(const int order)
            : System(order)
        {
        }

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
        void Update() override;

    private:
        friend class HierarchyWindow;

        struct SystemPtrComparer
        {
            bool operator()(const System* left, const System* right) const
            {
                if (left->GetOrder() == right->GetOrder())
                    return left < right; //确保顺序相同时依然有大小之分，从而避免不同系统实例被误认为相等
                return left->GetOrder() < right->GetOrder();
            }
        };

        ///
        /// 系统增删为什么要延迟执行？
        /// 添加或删除系统必须先缓存然后再实际执行，因为在遍历系统的时候是不能修改容器结构的，
        /// 但提供的游戏事件都是遍历容器的时候运行的，所以为了实现在系统事件中增删系统，必须先缓存
        /// 
        /// 虽然子系统的添加删除是延迟的，但并不支持遍历时的结构化更改。
        /// 例如Start时addingSystems被占用，但用户依然可能执行AddSubSystem函数，于是就会导致遍历异常。
        /// 此处延迟触发的真实原因是因为插入系统和删除系统是无序的，但系统本身是有序的，
        /// 为了满足系统的顺序安排，只有先缓存再汇总后才可知正确的执行顺序。
        std::vector<System*> systemsBuffer = {};
        std::set<System*, SystemPtrComparer> addingSystems = {};
        std::set<System*, SystemPtrComparer> updatingSystems = {};
        std::set<System*, SystemPtrComparer> removingSystems = {};

        void FlushAddingSystems();
        void FlushRemovingSystems();
        void FlushUpdatingSystems()
        {
            systemsBuffer.insert(systemsBuffer.end(), updatingSystems.begin(), updatingSystems.end());
            for (System* system : systemsBuffer)
            {
#ifdef GleamEngineEditor
                auto& name = system->GetName();
                Gleam_ProfilerSample(name);
#endif
                system->Update();
            }
            systemsBuffer.clear();
        }
    };

    template <class TGroup = void, int TMinOrder = SystemMinOrder, int TMaxOrder = SystemMaxOrder>
    class SystemGroupT : public SystemBaseT<SystemGroup, TGroup, TMinOrder, TMaxOrder>
    {
    };

    template <class TParentSystem, int Order>
    class AbsoluteSystemGroupT : public AbsoluteSystem<SystemGroup, TParentSystem, Order>
    {
    };

    template <class TBrotherSystem, SystemRelation Relation>
    class RelativeSystemGroupT : public RelativeSystem<SystemGroup, TBrotherSystem, Relation>
    {
    };
}