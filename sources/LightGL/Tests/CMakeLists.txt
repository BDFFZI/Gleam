addTests()

target_link_libraries("${ModuleName}" PUBLIC LightImport)

find_package(glm CONFIG REQUIRED)
target_link_libraries("${ModuleName}" PUBLIC glm::glm)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})