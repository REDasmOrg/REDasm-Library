cmake_minimum_required(VERSION 3.10)

project(LibREDasm)
include(GNUInstallDirs)
include(cmake/functions.cmake)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-DSHARED_OBJECT_EXT="${CMAKE_SHARED_LIBRARY_SUFFIX}")

# Capstone (Override Settings)
set(CAPSTONE_SRC impl/libs/capstone)
set(CAPSTONE_INCLUDE ${CAPSTONE_SRC}/include/capstone)
set(CAPSTONE_BUILD_TESTS  OFF CACHE BOOL "")
set(CAPSTONE_BUILD_SHARED OFF CACHE BOOL "")
set(CAPSTONE_BUILD_STATIC ON  CACHE BOOL "")
add_subdirectory(${CAPSTONE_SRC})

# API
file(GLOB_RECURSE REDASM_HEADERS redasm/*.h*)
file(GLOB_RECURSE REDASM_SOURCES redasm/*.cpp)

# Impl
file(GLOB_RECURSE IMPL_HEADERS
    impl/buffer/*.h*
    impl/database/*.h
    impl/disassembler/*.h
    impl/graph/*.h
    impl/libs/binutils_demangler/*.h
    impl/libs/dirent/*.h
    impl/libs/miniz/*.h
    impl/plugins/*.h
    impl/support/*.h
    context_impl.h)

file(GLOB_RECURSE IMPL_SOURCES
     impl/buffer/*.c*
     impl/database/*.c*
     impl/disassembler/*.c*
     impl/graph/*.c*
     impl/libs/binutils_demangler/*.c
     impl/libs/dirent/*.c
     impl/libs/miniz/*.c
     impl/plugins/*.c*
     impl/support/*.c*
     context_impl.cpp)

set(HEADERS ${REDASM_HEADERS} ${IMPL_HEADERS})
set(SOURCES ${REDASM_SOURCES} ${IMPL_SOURCES})

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} capstone-static)

target_include_directories(${PROJECT_NAME} PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                           $<INSTALL_INTERFACE:redasm>
                           PRIVATE impl)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Config
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})  # This is for Windows

if(PORTABLE_MODE)
    foreach(HEADER ${REDASM_HEADERS})
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" DST_HEADER ${HEADER})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${HEADER} "${CMAKE_BINARY_DIR}/${DST_HEADER}")
    endforeach()
else()
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/redasm DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN "*.h*")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(${PROJECT_NAME} pthread dl)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "LibREDasm")
