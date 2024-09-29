#pragma once
#include <string>
#include <vector>
#include <shaderc/shaderc.hpp>

namespace LightRuntime
{
    /**
     * https://www.khronos.org/assets/uploads/developers/library/2016-vulkan-devu-seoul/3-Making-SPIR-V-Modules.pdf
     */
    class ShaderImporter
    {
    public:
        static std::vector<std::byte> ImportHlsl(shaderc_shader_kind type, const std::string& code, const std::string& entryPoint);
    };
}
