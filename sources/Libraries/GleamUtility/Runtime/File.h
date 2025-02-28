#pragma once
#include <string>

namespace Gleam
{
    class File
    {
    public:
        static void WriteAllText(std::string_view filename, std::string_view content);
        static std::string ReadAllText(std::string_view filename);
    };
}