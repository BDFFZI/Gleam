#pragma once
#include <string>

namespace Gleam
{
    class File
    {
    public:
        static std::string ReadAllText(const std::string& filename);
    };
}