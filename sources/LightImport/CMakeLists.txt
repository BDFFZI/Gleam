addModule()

target_link_libraries("${ModuleName}" PUBLIC LightMath)

target_link_libraries("${ModuleName}" PUBLIC LightUtility)

find_package(Stb REQUIRED)
target_include_directories("${ModuleName}" PRIVATE ${Stb_INCLUDE_DIR})

find_package(tinyobjloader CONFIG REQUIRED)
target_link_libraries("${ModuleName}" PRIVATE tinyobjloader::tinyobjloader)

find_package(unofficial-shaderc CONFIG REQUIRED)
target_link_libraries("${ModuleName}" PRIVATE unofficial::shaderc::shaderc)