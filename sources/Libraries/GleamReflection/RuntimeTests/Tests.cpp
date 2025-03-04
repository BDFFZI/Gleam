#include <iostream>
#include <thread>
#include <fstream>
#include <ostream>
#include <string>
#include <typeindex>
#include <gtest/gtest.h>
#include <ranges>
#include <unordered_set>
#include <vector>

#include "Test.h"
#include "GleamReflection/Runtime/Type.h"

TEST(Reflection, Type)
{
    const Type& type = Type::CreateOrGet<CustomObject>();
    std::cout << type.GetID() << '\n';
    std::cout << type.GetName() << '\n';
    for (auto& fieldInfo : type.GetFields())
    {
        std::cout << fieldInfo.name << '\t';
        std::cout << fieldInfo.type.name() << '\t';
        std::cout << fieldInfo.offset << '\t';
        std::cout << fieldInfo.size << '\n';
    }
}