cmake_minimum_required(VERSION 3.10)

project(LibREDasm)
include(GNUInstallDirs)
include(cmake/functions.cmake)

if(UNIX AND NOT APPLE)
   find_program(CCACHE_PROGRAM ccache)
   if(CCACHE_PROGRAM AND (CMAKE_GENERATOR STREQUAL "Ninja" OR CMAKE_GENERATOR STREQUAL "Unix Makefiles"))
       message(STATUS "${PROJECT_NAME}: Found CCache, speed up compilation")
       set(CMAKE_C_COMPILER_LAUNCHER   ${CCACHE_PROGRAM})
       set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
   endif()

  execute_process(COMMAND ${CMAKE_CXX_COMPILER} -fuse-ld=gold -Wl,--version ERROR_QUIET OUTPUT_VARIABLE ld_version)
  if("${ld_version}" MATCHES "GNU gold")
      message(STATUS "${PROJECT_NAME}: Found Gold linker, use faster linker")
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")
      set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=gold ")
  endif()
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-DSHARED_OBJECT_EXT="${CMAKE_SHARED_LIBRARY_SUFFIX}")

# Core
file(GLOB_RECURSE RDCORE_HEADERS rdcore/*.h*)
file(GLOB_RECURSE RDCORE_SOURCES rdcore/*.c*)

# API
file(GLOB_RECURSE RDAPI_HEADERS rdapi/*.h*)
file(GLOB_RECURSE RDAPI_SOURCES rdapi/*.c*)

set(HEADERS ${RDCORE_HEADERS} ${RDAPI_HEADERS})
set(SOURCES ${RDCORE_SOURCES} ${RDAPI_SOURCES})

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Config
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})  # This is for Windows

if(PORTABLE_MODE)
    # Create "rdapi" folder
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/rdapi")

    # Copy Headers
    foreach(HEADER ${RDAPI_HEADERS})
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" DST_HEADER ${HEADER})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${HEADER} "${CMAKE_BINARY_DIR}/${DST_HEADER}")
    endforeach()

    # Copy FindREDasm.cmake
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
                       "${CMAKE_CURRENT_SOURCE_DIR}/rdapi/FindREDasm.cmake" "${CMAKE_BINARY_DIR}/rdapi")
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
