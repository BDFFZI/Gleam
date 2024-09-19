addTests()

find_package(magic_enum CONFIG REQUIRED)
target_link_libraries("${ModuleName}" PRIVATE magic_enum::magic_enum)