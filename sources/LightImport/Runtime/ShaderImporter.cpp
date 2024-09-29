#include "ShaderImporter.h"

#include <stdexcept>

std::vector<std::byte> GetCompilationResult(const shaderc::SpvCompilationResult& compilationResult)
{
    if (compilationResult.GetCompilationStatus() == shaderc_compilation_status_compilation_error)
    {
        std::string errorMessage = compilationResult.GetErrorMessage();
        throw std::runtime_error("编译着色器失败！\n" + errorMessage);
    }

    std::vector bytecode(
        reinterpret_cast<const std::byte*>(compilationResult.begin()),
        reinterpret_cast<const std::byte*>(compilationResult.end())
    );
    return bytecode;
}

using namespace LightRuntime;

std::vector<std::byte> ShaderImporter::ImportHlsl(const shaderc_shader_kind type, const std::string& code, const std::string& entryPoint)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetSourceLanguage(shaderc_source_language_hlsl);
    options.SetOptimizationLevel(shaderc_optimization_level_performance);

    auto compilationResult = compiler.CompileGlslToSpv(code, type, "[shader]", entryPoint.c_str(), options);
    return GetCompilationResult(compilationResult);
}
