function(redasm_plugin P_NAME)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_SKIP_BUILD_RPATH TRUE)
    string(TOLOWER "${P_NAME}" P_ID)
    add_definitions(-Dr_plugin=${P_ID})
    add_definitions(-Dr_plugin_id="${P_ID}")

    cmake_parse_arguments(ARG "${OPTIONS}" "" "" ${ARGN})
    add_library(${P_NAME} SHARED ${ARG_UNPARSED_ARGUMENTS})

    if(DEFINED REDASM_INCLUDE_PATH)
        target_include_directories(${P_NAME} PRIVATE ${REDASM_INCLUDE_PATH})
    endif()

    if(DEFINED REDASM_LIBRARY_PATH)
        target_link_libraries(${P_NAME} PRIVATE "${REDASM_LIBRARY_PATH}/LibREDasm${CMAKE_SHARED_LIBRARY_SUFFIX}")
    else()
        target_link_libraries(${P_NAME} PRIVATE LibREDasm)
    endif()

    set_target_properties(${P_NAME} PROPERTIES PREFIX "")
    set_target_properties(${P_NAME} PROPERTIES OUTPUT_NAME "${P_ID}")
    set_target_properties(${P_NAME} PROPERTIES LINKER_LANGUAGE CXX)
endfunction()
