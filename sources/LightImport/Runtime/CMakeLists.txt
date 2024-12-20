addModule()

target_link_libraries("${ProjectName}" PRIVATE LightMath)

target_link_libraries("${ProjectName}" PRIVATE LightUtility)

find_package(Stb REQUIRED)
target_include_directories("${ProjectName}" PRIVATE ${Stb_INCLUDE_DIR})

find_package(tinyobjloader CONFIG REQUIRED)
target_link_libraries("${ProjectName}" PRIVATE tinyobjloader::tinyobjloader)

find_package(unofficial-shaderc CONFIG REQUIRED)
target_link_libraries("${ProjectName}" PRIVATE unofficial::shaderc::shaderc)