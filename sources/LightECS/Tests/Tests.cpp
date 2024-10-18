#include <vector>
template <class...>
struct Archetype
{
};

template <class...>
void RegisterArchetype()
{
}

struct Position
{
    float x;
    float y;
};

struct Velocity
{
    float x;
    float y;
};

struct World
{
    template <class...>
    static void AddEntities()
    {
    }

    template <class...>
    static void RemoveEntities()
    {
    }

    template <class...>
    static void ForeachEntities()
    {
    }

    inline static std::vector<> positions = {};
};

template <Position, Velocity>
struct Archetype
{
};

void main()
{
    World::AddEntity<Position, Velocity>();
}
