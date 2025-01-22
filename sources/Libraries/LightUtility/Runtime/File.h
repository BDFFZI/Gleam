#pragma once
#include <string>

namespace Light
{
    class File
    {
    public:
        static std::string ReadAllText(const std::string& filename);
    };
}