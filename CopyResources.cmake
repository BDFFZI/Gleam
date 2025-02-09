message("Run resources folder copying task.")

foreach(ResourcesPath ${ResourcesPath})
    file(COPY ${ResourcesPath} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    message("Copy:${ResourcesPath}")
endforeach()
