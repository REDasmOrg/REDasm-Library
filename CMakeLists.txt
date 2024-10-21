cmake_minimum_required(VERSION 3.12)

project(LibREDasm)
include(cmake/functions.cmake)
include(cmake/installdirs.cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
option(RDAPI_DOC "Build API Documentation" ON)
option(ENABLE_TESTS "Enable Unit Testing" ON)
add_definitions(-DSHARED_OBJECT_EXT="${CMAKE_SHARED_LIBRARY_SUFFIX}" -DMINIZ_NO_ZLIB_COMPATIBLE_NAMES)

# set(CPM_USE_LOCAL_PACKAGES ON)
include(cmake/get_cpm.cmake) # https://github.com/TheLartians/CPM.cmake
CPMAddPackage("gh:gabime/spdlog@1.9.2")
CPMAddPackage("gh:nlohmann/json@3.10.5")

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif(ENABLE_TESTS)

if(RDAPI_DOC)
    check_documentation()
endif(RDAPI_DOC)

check_ccache_and_ld()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(Threads REQUIRED)
endif()

if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601) # Try to keep Windows 7 compatibility
endif()

# Core
file(GLOB_RECURSE LIBS_HEADERS
     rdcore/libs/binutils_demangler/*.h* rdcore/libs/miniz/*.h* rdcore/libs/taojson/include/*.h*
     rdcore/libs/undname.h)

file(GLOB_RECURSE LIBS_SOURCES
     rdcore/libs/binutils_demangler/*.c* rdcore/libs/miniz/*.c* rdcore/libs/taojson/include/*.c*)

file(GLOB_RECURSE RDCORE_HEADERS
     rdcore/buffer/*.h* rdcore/builtin/*.h* rdcore/containers/*.h*
     rdcore/database/*.h* rdcore/document/*.h* rdcore/engine/*.h*
     rdcore/graph/*.h* rdcore/plugin/*.h* rdcore/rdil/*.h*
     rdcore/serializer/*.h* rdcore/surface/*.h*
     rdcore/support/*.h* rdcore/types/*.h* rdcore/config.h
     rdcore/context.h rdcore/disassembler.h rdcore/object.h)

file(GLOB_RECURSE RDCORE_SOURCES
     rdcore/buffer/*.c* rdcore/builtin/*.c* rdcore/containers/*.c*
     rdcore/database/*.c* rdcore/document/*.c* rdcore/engine/*.c*
     rdcore/graph/*.c* rdcore/plugin/*.c* rdcore/rdil/*.c*
     rdcore/serializer/*.c* rdcore/surface/*.c*
     rdcore/support/*.c* rdcore/types/*.c* rdcore/config.cpp
     rdcore/context.cpp rdcore/disassembler.cpp rdcore/object.cpp)

# API
file(GLOB_RECURSE RDAPI_HEADERS rdapi/*.h*)
file(GLOB_RECURSE RDAPI_SOURCES rdapi/*.c*)

set(HEADERS ${LIBS_HEADERS} ${RDCORE_HEADERS} ${RDAPI_HEADERS})
set(SOURCES ${LIBS_SOURCES} ${RDCORE_SOURCES} ${RDAPI_SOURCES})

add_subdirectory(rdcore/libs/taojson EXCLUDE_FROM_ALL)
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE taocpp::json spdlog nlohmann_json::nlohmann_json)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

set_target_properties(${PROJECT_NAME} PROPERTIES
                      POSITION_INDEPENDENT_CODE ON
                      CXX_EXTENSIONS OFF
                      OUTPUT_NAME "LibREDasm"
                      PREFIX "")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads ${CMAKE_DL_LIBS})
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug") # Enforce warnings as errors if not in Release mode
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX /permissive)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror -Wextra -Wno-error=extra -Wshadow -pedantic)
    endif()
endif()

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Config
        ARCHIVE DESTINATION ${REDASM_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${REDASM_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${REDASM_INSTALL_BINDIR})  # This is for Windows

install(DIRECTORY rdapi
        DESTINATION ${REDASM_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h")

install(DIRECTORY rdapi
        DESTINATION ${REDASM_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h")

install(FILES rdapi/FindREDasm.cmake
        DESTINATION ${REDASM_CMAKE_PATH})
