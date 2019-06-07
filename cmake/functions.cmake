function(disable_warnings_for HEADERS SOURCES)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set_source_files_properties(${HEADERS} PROPERTIES COMPILE_OPTIONS -w)
        set_source_files_properties(${SOURCES} PROPERTIES COMPILE_OPTIONS -w)
    endif()
endfunction()
 
