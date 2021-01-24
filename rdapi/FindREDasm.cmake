set(REDASM_FOUND TRUE)
set(REDASM_LIBRARY_NAME "LibREDasm${CMAKE_SHARED_LIBRARY_SUFFIX}")
set(LOADER_TYPE "loaders")
set(ASSEMBLER_TYPE "assemblers")
set(PLUGIN_TYPE "plugins")

foreach(MODULE ${CMAKE_MODULE_PATH})
    if(NOT DEFINED REDASM_INCLUDE_PATH AND EXISTS "${MODULE}/rdapi.h")
        set(REDASM_INCLUDE_PATH "${MODULE}/..")
    endif()

    if(NOT DEFINED REDASM_LIBRARY_PATH AND EXISTS "${MODULE}/../${REDASM_LIBRARY_NAME}")
        set(REDASM_LIBRARY_PATH "${MODULE}/../${REDASM_LIBRARY_NAME}")
    endif()
endforeach()

if(WIN32)
    if(NOT DEFINED REDASM_INCLUDE_PATH)
        set(REDASM_INCLUDE_PATH ${CMAKE_CURRENT_LIST_DIR})
    endif()

    if(NOT DEFINED REDASM_LIBRARY_PATH)
        set(REDASM_LIBRARY_PATH ${CMAKE_BINARY_DIR})
    endif()
endif()

function(redasm_plugin P_NAME P_TYPE)
    set(CMAKE_SKIP_BUILD_RPATH TRUE)
    string(TOLOWER "${P_NAME}" P_ID)

    cmake_parse_arguments(ARG "${OPTIONS}" "" "" ${ARGN})
    add_library(${P_NAME} SHARED ${ARG_UNPARSED_ARGUMENTS})

    if(DEFINED REDASM_INCLUDE_PATH)
        target_include_directories(${P_NAME} PRIVATE ${REDASM_INCLUDE_PATH})
    endif()

    if(DEFINED REDASM_LIBRARY_PATH)
        target_link_directories(${P_NAME} PRIVATE ${REDASM_LIBRARY_PATH})
    endif()

    set_target_properties(${P_NAME} PROPERTIES
                                    POSITION_INDEPENDENT_CODE ON
                                    CXX_EXTENSIONS OFF
                                    PREFIX ""
                                    OUTPUT_NAME "${P_ID}"
                                    LINKER_LANGUAGE CXX)

    target_link_libraries(${P_NAME} PRIVATE LibREDasm)
    target_compile_definitions(${P_NAME} PRIVATE -Drd_plugin_id="${P_ID}")
    target_compile_features(${P_NAME} PUBLIC cxx_std_17)

    # HACK: Set install directory manually
    if(WIN32)
        install(TARGETS ${P_NAME}
            LIBRARY DESTINATION "plugins/${P_TYPE}"
            RUNTIME DESTINATION "plugins/${P_TYPE}")  # This is for Windows
    else()
        include(GNUInstallDirs)
        install(TARGETS ${P_NAME} DESTINATION "${REDASM_INSTALL_PATH}/plugins/${P_TYPE}")
    endif()
endfunction()
