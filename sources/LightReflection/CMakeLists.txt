addModule()

find_package(stduuid CONFIG REQUIRED)
target_link_libraries("${ModuleName}" PUBLIC stduuid)