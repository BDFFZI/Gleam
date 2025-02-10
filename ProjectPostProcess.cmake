message("后处理项目：${AllProjects}")

foreach(ProjectName ${AllProjects})
    # 统计所有依赖项
    set(RealDependencies "")

    set(SourceDependencies ${${ProjectName}Dependencies})

    while(NOT SourceDependencies STREQUAL "") # 递归添加所有依赖项
        set(Dependencies ${SourceDependencies})
        set(SourceDependencies "")

        foreach(dependency ${Dependencies})
            list(PREPEND RealDependencies ${dependency})
            list(APPEND SourceDependencies ${${dependency}Dependencies})
        endforeach()
    endwhile()

    if(NOT RealDependencies STREQUAL "")
        list(REMOVE_DUPLICATES RealDependencies)
        set(${ProjectName}Dependencies ${RealDependencies} CACHE STRING "项目依赖的其他项目" FORCE)
        message("发现依赖项目：${ProjectName}->${${ProjectName}Dependencies}")
    endif()

    # 统计所有依赖资源
    set(ALLResourcesPaths "")

    foreach(dependency ${${ProjectName}Dependencies})
        # 查看依赖的Resources文件
        file(GLOB_RECURSE ResourcesPaths LIST_DIRECTORIES true "${${dependency}Source}/[!.]")
        list(FILTER ResourcesPaths INCLUDE REGEX "Resources$")
        list(APPEND ALLResourcesPaths ${ResourcesPaths})
    endforeach()

    if(NOT ALLResourcesPaths STREQUAL "")
        set(${ProjectName}Resources ${ALLResourcesPaths} CACHE STRING "项目依赖的资源" FORCE)
        message("发现依赖资源：${ProjectName}->${${ProjectName}Resources}")
    endif()

    get_target_property(ProjectType ${ProjectName} TYPE) # 获取项目类型

    # 可执行项目作为最终编译目标需要进行一些处理
    if(${ProjectType} STREQUAL "EXECUTABLE")
        # 处理库初始化文件
        foreach(dependency ${${ProjectName}Dependencies})
            set(LibraryInitFile "${${dependency}Source}/__Init__.h") # 库初始化文件

            # 目标库存在初始化文件，需要记录到项目初始化文件中
            if(EXISTS ${LibraryInitFile})
                set(ProjectInitFile "${${ProjectName}Source}/__Init__.cpp") # 项目初始化地址
                file(RELATIVE_PATH relative_path "${${dependency}Source}/../.." ${LibraryInitFile})
                file(APPEND ${ProjectInitFile} "#include \"${relative_path}\"\n")
            endif()
        endforeach()

        # 处理资源依赖
        get_target_property(OUTPUT_DIRECTORY ${ProjectName} RUNTIME_OUTPUT_DIRECTORY)

        foreach(ResourcesPath ${ALLResourcesPaths})
            file(COPY ${ResourcesPath} DESTINATION ${OUTPUT_DIRECTORY})
        endforeach()
    endif()
endforeach()