#pragma once
#include <format>
#include <array>
#include <string>
#include <vector>
#include <unordered_map>

class Archetype
{
public:
    inline static std::vector<const Archetype*> allArchetypes = {};

    virtual ~Archetype() = default;
    virtual std::string GetName() const = 0;
    virtual size_t GetSize() const = 0;
    virtual int GetComponentCount() const = 0;
    virtual std::vector<const type_info*> GetComponentTypes() const = 0;
    virtual std::vector<int> GetComponentOffsets() const = 0;
    virtual void RunConstructor(std::byte* ptr) const = 0;
    virtual void RunDestructor(std::byte* ptr) const = 0;
    virtual std::unordered_map<const type_info*, int>& GetMemoryMap() const =0;
    virtual std::string ToString() const = 0;

    template <class... TComponents>
    bool Contains() const
    {
        const type_info* components[] = {&typeid(TComponents)...};
        std::unordered_map<const type_info*, int>& map = GetMemoryMap();
        for (int i = 0; i < sizeof...(TComponents); ++i)
            if (map.contains(components[i]) == false)
                return false;
        return true;
    }
    template <class... TComponents>
    std::array<int, sizeof...(TComponents)> MemoryMap() const
    {
        std::unordered_map<const type_info*, int>& map = GetMemoryMap();
        return {map.at(&typeid(TComponents))...};
    }
};

#include "ArchetypeMeta.hpp"

#define MakeArchetype(name,...)\
constexpr ArchetypeMeta name##Meta = CreateArchetypeMeta<Entity,__VA_ARGS__>(#name);\
constexpr const Archetype* name = &name##Meta;\
int name##ID = Archetype::allArchetypes.size();\
int name##Register = [](){Archetype::allArchetypes.emplace_back(name);return 0;}();

#define MakeArchetypeInherited(name,parent,...)\
constexpr ArchetypeMeta name##Meta = CreateArchetypeMeta<Entity,__VA_ARGS__>(#name,parent##Meta);\
constexpr const Archetype* name = &name##Meta;\
int name##ID = Archetype::allArchetypes.size();\
int name##Register = [](){Archetype::allArchetypes.emplace_back(name);return 0;}();
