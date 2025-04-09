#pragma once
#include "GleamECS/Runtime/World/World.h"

namespace Gleam
{
    struct Entity
    {
        uint32_t id;
        World* world;

        auto operator<=>(const Entity& other) const = default;
    };
}

template <>
struct std::hash<Gleam::Entity>
{
    size_t operator()(const Gleam::Entity& entity) const noexcept
    {
        return std::hash<uint32_t>()(entity.id) ^ std::hash<Gleam::World*>()(entity.world);
    }
};