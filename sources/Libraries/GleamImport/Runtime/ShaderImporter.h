#pragma once
#include <string>
#include <vector>
#include <shaderc/shaderc.hpp>

namespace Gleam
{
    /**
     * https://www.khronos.org/assets/uploads/developers/library/2016-vulkan-devu-seoul/3-Making-SPIR-V-Modules.pdf
     */
    class ShaderImporter
    {
    public:
        static std::vector<std::byte> ImportHlsl(const std::string& code, shaderc_shader_kind type, const std::string& entryPoint);
        static std::vector<std::byte> ImportHlslFromFile(const std::string& file, shaderc_shader_kind type, const std::string& entryPoint);
    };
}
