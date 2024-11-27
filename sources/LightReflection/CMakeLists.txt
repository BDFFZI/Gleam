addModule()

find_package(stduuid REQUIRED)
target_include_directories("${ModuleName}" PUBLIC "${Libuuid_INCLUDE_DIRS}")

find_path(Libuuid_INCLUDE_DIRS uuid.h)
message("目录： ${Libuuid_INCLUDE_DIRS}")