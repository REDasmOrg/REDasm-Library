cmake_minimum_required(VERSION 3.10)

project(LibREDasm)
include(GNUInstallDirs)
include(cmake/functions.cmake)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-DSHARED_OBJECT_EXT="${CMAKE_SHARED_LIBRARY_SUFFIX}")

# Some libraries needs this relative directory
set(IMPL_LIBS_INCLUDE   impl/libs)
set(REDASM_LIBS_INCLUDE redasm/libs)

# Capstone (Override Settings)
set(CAPSTONE_SRC ${REDASM_LIBS_INCLUDE}/capstone)
set(CAPSTONE_INCLUDE ${CAPSTONE_SRC}/include/capstone)
set(CAPSTONE_BUILD_TESTS  OFF CACHE BOOL "")
set(CAPSTONE_BUILD_SHARED OFF CACHE BOOL "")
set(CAPSTONE_BUILD_STATIC ON  CACHE BOOL "")
add_subdirectory(${CAPSTONE_SRC})

# Impl
file(GLOB_RECURSE IMPL_HEADERS impl/*.h*)
file(GLOB_RECURSE IMPL_SOURCES impl/*.c*)

# API
file(GLOB_RECURSE REDASM_HEADERS
    redasm/buffer/*.h*
    redasm/database/*.h
    redasm/disassembler/*.h
    redasm/graph/*.h
    redasm/libs/json/*.h*
    redasm/libs/visit_struct/*.h*
    redasm/plugins/*.h
    redasm/static/*.h
    redasm/support/*.h
    redasm/types/*.h
    context.h level.h macros.h pimpl.h redasm.h ui.h)

file(GLOB_RECURSE REDASM_SOURCES
     redasm/buffer/*.c*
     redasm/database/*.c*
     redasm/disassembler/*.c*
     redasm/graph/*.c*
     redasm/plugins/*.c*
     redasm/support/*.c*
     redasm/types/*.c*
     context.cpp ui.cpp)

set(HEADERS ${REDASM_HEADERS} ${IMPL_HEADERS})
set(SOURCES ${REDASM_SOURCES} ${IMPL_SOURCES})

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} capstone-static)

target_include_directories(${PROJECT_NAME}
                           PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                   $<INSTALL_INTERFACE:redasm>
                                   ${CAPSTONE_INCLUDE}
                           PRIVATE ${IMPL_LIBS_INCLUDE}
                                   impl)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Config
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})  # This is for Windows

if(PORTABLE_MODE)
    # Create "redasm" folder
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/redasm")

    # Copy Headers
    foreach(HEADER ${REDASM_HEADERS})
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" DST_HEADER ${HEADER})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${HEADER} "${CMAKE_BINARY_DIR}/${DST_HEADER}")
    endforeach()

    # Copy FindREDasm.cmake
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
                       "${CMAKE_CURRENT_SOURCE_DIR}/redasm/FindREDasm.cmake" "${CMAKE_BINARY_DIR}/redasm")

    # Copy capstone headers
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
                       "${CMAKE_CURRENT_SOURCE_DIR}/redasm/libs/capstone/include/capstone" "${CMAKE_BINARY_DIR}/redasm/libs/capstone")
else()
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/redasm DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN "*.h*")
endif()

if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") AND DEBUG_STL_ITERATORS)
    target_compile_options(${PROJECT_NAME} PRIVATE -D_GLIBCXX_DEBUG)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wno-missing-braces)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(${PROJECT_NAME} pthread dl)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "LibREDasm")
