addLibrary()

find_package(stduuid CONFIG REQUIRED)
target_link_libraries(${ProjectName} PRIVATE stduuid)
