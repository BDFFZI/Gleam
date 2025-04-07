#pragma once
#include "GleamReflection/Runtime/Type.h"
#include "GleamECS/Runtime/Entity/Archetype.h"

struct Transform
{
    float position;

    friend bool operator==(const Transform& lhs, const Transform& rhs)
    {
        return abs(lhs.position - rhs.position) < std::numeric_limits<float>::epsilon();
    }
};

Gleam_MakeTypeWithID(Transform, "00000000-0001-435E-AE61-D12757C441AE")
{
    Gleam_MakeType_AddField(position);
}

struct RigidBody
{
    float force;
    float mass = 1;
    float velocity;

    friend bool operator==(const RigidBody& lhs, const RigidBody& rhs)
    {
        return abs(lhs.force - rhs.force) < std::numeric_limits<float>::epsilon()
            && abs(lhs.mass - rhs.mass) < std::numeric_limits<float>::epsilon()
            && abs(lhs.velocity - rhs.velocity) < std::numeric_limits<float>::epsilon();
    }
};

Gleam_MakeTypeWithID(RigidBody, "00000000-0002-435E-AE61-D12757C441AE")
{
    Gleam_MakeType_AddField(force);
    Gleam_MakeType_AddField(mass);
    Gleam_MakeType_AddField(velocity);
}

struct SpringPhysics
{
    float pinPosition = 0;
    float length = 5;
    float elasticity = 200;

    friend bool operator==(const SpringPhysics& lhs, const SpringPhysics& rhs)
    {
        return abs(lhs.pinPosition - rhs.pinPosition) < std::numeric_limits<float>::epsilon()
            && abs(lhs.length - rhs.length) < std::numeric_limits<float>::epsilon()
            && abs(lhs.elasticity - rhs.elasticity) < std::numeric_limits<float>::epsilon();
    }
};

Gleam_MakeTypeWithID(SpringPhysics, "00000000-0003-435E-AE61-D12757C441AE")
{
    Gleam_MakeType_AddField(pinPosition);
    Gleam_MakeType_AddField(length);
    Gleam_MakeType_AddField(elasticity);
}

Gleam_MakeArchetype(physicsArchetype, Transform, RigidBody)
Gleam_MakeArchetypeChild(physicsWithSpringArchetype, physicsArchetype, SpringPhysics)
