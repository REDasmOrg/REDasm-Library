function(redasm_plugin P_NAME)
    set(CMAKE_CXX_STANDARD 11)
    string(TOLOWER "${P_NAME}" P_ID)
    add_definitions(-Dr_plugin=${P_ID})
    add_definitions(-Dr_plugin_id="${P_ID}")

    cmake_parse_arguments(ARG "${OPTIONS}" "" "" ${ARGN})
    add_library(${P_NAME} SHARED ${ARG_UNPARSED_ARGUMENTS})

    if(DEFINED REDASM_API_PATH)
        target_include_directories(${P_NAME} PRIVATE ${REDASM_API_PATH})
    endif()

    target_link_libraries(${P_NAME} PRIVATE LibREDasm)
    set_target_properties(${P_NAME} PROPERTIES PREFIX "")
    set_target_properties(${P_NAME} PROPERTIES OUTPUT_NAME "${P_ID}")
    set_target_properties(${P_NAME} PROPERTIES LINKER_LANGUAGE CXX)
endfunction()
