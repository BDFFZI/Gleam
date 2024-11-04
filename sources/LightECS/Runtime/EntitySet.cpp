#include "EntitySet.h"

EntitySet::EntitySet()
{
    for (const auto& archetype : Archetype::allArchetypes)
        entityHeaps.emplace_back(archetype->GetSize());
}
void EntitySet::AddEntity(const int archetypeID)
{
    entityHeaps[archetypeID].AddElement([archetypeID](std::byte* item)
    {
        Archetype::allArchetypes[archetypeID]->RunConstructor(item);
    });
}
void EntitySet::AddEntities(const int archetypeID, const int count)
{
    entityHeaps[archetypeID].AddElements(count, [archetypeID](int itemIndex, std::byte* item)
    {
        Archetype::allArchetypes[archetypeID]->RunConstructor(item);
    });
}
