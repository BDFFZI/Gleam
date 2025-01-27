// #include <gtest/gtest.h>
//
// #include "LightEngine/Runtime/Engine.h"
// #include "LightReflection/RuntimeTests/Test.h"
//
// using namespace Light;
//
// Light_MakeArchetype(DataArchetype, TestData)
//
// int main()
// {
//     Entity entity = Awake->AddEntity(DataArchetype);
//     World::SetComponents(entity, data);
//
//     Awake->AddEntity(VectorArchetype);
//
//     Engine::Start();
//     return 0;
// }

#include <gtest/gtest.h>

#include "LightEngine/Editor/EditorUI/EditorUISerializer.h"
#include "LightEngine/Runtime/Engine.h"
#include "LightReflection/Runtime/Type.h"
#include "LightUI/Runtime/UISystem.h"

struct Vector
{
    std::vector<int64_t> data = {101, 102, 103};
};

Light_MakeArchetype(VectorArchetype, Vector)

void main()
{
    // std::vector<int64_t> small = std::vector<int64_t>(1);
    // std::uintptr_t smallAddress = reinterpret_cast<std::uintptr_t>(&small);
    // std::uintptr_t smallData = reinterpret_cast<std::uintptr_t>(small.data());
    //
    // std::vector<int64_t> big = std::vector<int64_t>(1000);
    // std::uintptr_t bigAddress = reinterpret_cast<std::uintptr_t>(&big);
    // std::uintptr_t bigData = reinterpret_cast<std::uintptr_t>(big.data());
    std::cout << std::is_trivially_copyable_v<std::vector<int64_t>> << std::endl;
    std::vector<int64_t> source = {101, 102, 103};
    std::byte destination[sizeof(std::vector<int64_t>)];
    std::memcpy(destination, &source, sizeof(std::vector<int64_t>));
    std::vector<int64_t>& result = *reinterpret_cast<std::vector<int64_t>*>(destination);

    std::vector<int64_t>* sourceAddress = &source;
    std::vector<int64_t>* resultAddress = &result;
    
    result.resize(4);
    int64_t value = *result.begin();
    std::cout << "value: " << value << std::endl;


    // using namespace Light;
    // Entity entity = Awake->AddEntity(VectorArchetype);
    // std::uintptr_t source =reinterpret_cast<uintptr_t>(&World::GetComponent<Vector>(entity).data);
    //
    // SystemEvent systemEvent = {"Test", UISystem};
    // systemEvent.onUpdate = [entity,source]
    // {
    //     if (Time->GetTime() > 3)
    //     {
    //         std::uintptr_t destination =reinterpret_cast<uintptr_t>(&World::GetComponent<Vector>(entity).data);
    //         
    //         std::vector<int64_t>& entityVector = World::GetComponent<Vector>(entity).data;
    //         entityVector.resize(4);
    //         int64_t value = *entityVector.begin();
    //     }
    //
    //     std::cout << World::GetComponent<Vector>(entity).data.size() << std::endl;
    // };
    //
    // World::AddSystem(&systemEvent);
    //
    // Engine::Start();
}