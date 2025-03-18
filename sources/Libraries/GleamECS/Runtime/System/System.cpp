#include "System.h"

namespace Gleam
{
    std::optional<std::reference_wrapper<System>> System::GetGlobalSystem(const uuids::uuid id)
    {
        if (allGlobalSystems.contains(id))
            return *allGlobalSystems.at(id);
        return std::nullopt;
    }

    System::System()
        : System(std::nullopt)
    {
    }
    System::System(
        const std::optional<std::reference_wrapper<SystemGroup>> group,
        const int minOrder, const int maxOrder, const std::string_view name)
        : name(name), group(group), minOrder(minOrder), maxOrder(maxOrder),
          order(static_cast<int32_t>((static_cast<int64_t>(minOrder) + static_cast<int64_t>(maxOrder)) / 2))
    {
    }
    System::System(System& system, const OrderRelation orderRelation, const std::string_view name)
        : System(system.GetGroup(),
                 orderRelation == OrderRelation::Before ? system.minOrder : system.order,
                 orderRelation == OrderRelation::Before ? system.order : system.maxOrder, name)
    {
    }
    System::System(SystemGroup& group)
        : System(std::optional<std::reference_wrapper<SystemGroup>>(group))
    {
    }
    const std::string& System::GetName() const
    {
        return name;
    }
    uuids::uuid System::GetID() const
    {
        return id;
    }
    std::optional<std::reference_wrapper<SystemGroup>> System::GetGroup() const
    {
        return group;
    }
    int System::GetOrder() const
    {
        return order;
    }
    void System::Start()
    {
    }
    void System::Stop()
    {
    }
    void System::Update()
    {
    }
}