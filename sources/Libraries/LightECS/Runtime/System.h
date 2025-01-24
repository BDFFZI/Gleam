﻿#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <ranges>
#include <set>

namespace Light
{
    enum class OrderRelation:uint8_t
    {
        Before,
        After
    };

    class SystemGroup;
    class System
    {
    public:
        constexpr static int32_t LeftOrder = std::numeric_limits<int32_t>::lowest();
        constexpr static int32_t RightOrder = std::numeric_limits<int32_t>::max();
        constexpr static int32_t MiddleOrder = 0;

        template <typename TSystem>
        static TSystem* Register()
        {
            TSystem* system = new TSystem();
            allSystems.emplace_back(system);
            return system;
        }

        explicit System(std::optional<SystemGroup*> group, int minOrder = LeftOrder, int maxOrder = RightOrder);
        System(System* system, OrderRelation orderRelation);
        virtual ~System() = default;

        std::optional<SystemGroup*> GetGroup() const;
        int GetOrder() const;

        virtual void Start();
        virtual void Stop();
        virtual void Update();

    private:
        static inline std::vector<std::unique_ptr<System>> allSystems;

        std::optional<SystemGroup*> group;
        int minOrder;
        int maxOrder;
        int order;
    };

    class SystemEvent : public System
    {
    public:
        std::function<void()> onStart = nullptr;
        std::function<void()> onStop = nullptr;
        std::function<void()> onUpdate = nullptr;

        SystemEvent(std::optional<SystemGroup*> group, int minOrder = LeftOrder, int maxOrder = RightOrder);
        SystemEvent(System* system, OrderRelation orderRelation);

    private:
        void Start() override;
        void Stop() override;
        void Update() override;
    };

    struct SystemPtrComparer
    {
        bool operator()(const System* left, const System* right) const
        {
            if (left->GetOrder() == right->GetOrder())
                return left < right; //确保顺序相同时依然有大小之分，从而避免被误认为相等
            return left->GetOrder() < right->GetOrder();
        }
    };

    class SystemGroup : public System
    {
    public:
        SystemGroup(std::optional<SystemGroup*> group, int minOrder = LeftOrder, int maxOrder = RightOrder);
        SystemGroup(System* system, OrderRelation orderRelation);

        void AddSubSystem(System* system);
        void RemoveSubSystem(System* system);

        void Start() override;
        void Stop() override;
        void Update() override;

    private:
        friend class EditorUIUtility;

        std::set<System*, SystemPtrComparer> subSystemStartQueue = {};
        std::set<System*, SystemPtrComparer> subSystemStopQueue = {};
        std::set<System*, SystemPtrComparer> subSystemUpdateQueue = {};
    };

#define Light_MakeSystem(systemClass) \
    inline systemClass* systemClass = Light::System::Register<class systemClass##>();
}