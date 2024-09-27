@REM "D:/Vulkan SDK/Bin/glslc.exe" shader.vert -o vert.spv
@REM "D:/Vulkan SDK/Bin/glslc.exe" shader.frag -o frag.spv
"D:/Vulkan SDK/Bin/dxc.exe" -spirv -T vs_6_0 -E VertexShader shader.hlsl -Fo vertexShader.spv
"D:/Vulkan SDK/Bin/dxc.exe" -spirv -T ps_6_0 -E FragmentShader shader.hlsl -Fo fragmentShader.spv
pause