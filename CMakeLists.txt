cmake_minimum_required(VERSION 3.10)

project(LibREDasm)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(DEPENDS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/depends)
set(DEPENDS_BUILD_DIR ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/depends)
set(DEPENDS_HEADERS_DIR ${DEPENDS_SRC_DIR}/include)

### Dependencies

function(disable_warnings_for HEADERS SOURCES)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set_source_files_properties(${HEADERS} PROPERTIES COMPILE_OPTIONS -w)
        set_source_files_properties(${SOURCES} PROPERTIES COMPILE_OPTIONS -w)
    endif()
endfunction()

# Single Headers
set(DEPENDS_SINGLE_HEADERS ${DEPENDS_HEADERS_DIR}
                           ${DEPENDS_HEADERS_DIR}/json
                           ${DEPENDS_HEADERS_DIR}/visit_struct
                           ${DEPENDS_HEADERS_DIR}/binutils_demangler
                           ${DEPENDS_SRC_DIR}/miniz)

# MiniZ
set(MINIZ_HEADERS ${DEPENDS_SRC_DIR}/miniz/miniz.h)
set(MINIZ_SOURCES ${DEPENDS_SRC_DIR}/miniz/miniz.c)
disable_warnings_for("${MINIZ_HEADERS}" "${MINIZ_SOURCES}")

# Binutils Demangler (libiberty)
set(BINUTILS_DEMANGLER_DIR     ${DEPENDS_HEADERS_DIR}/binutils_demangler)
set(BINUTILS_DEMANGLER_HEADERS ${BINUTILS_DEMANGLER_DIR}/demangle.h ${BINUTILS_DEMANGLER_DIR}/cp-demangle.h)
set(BINUTILS_DEMANGLER_SOURCES ${BINUTILS_DEMANGLER_DIR}/cp-demangle.c)
disable_warnings_for("${BINUTILS_DEMANGLER_HEADERS}" "${BINUTILS_DEMANGLER_SOURCES}")

# Capstone (Override Settings)
set(CAPSTONE_SRC ${DEPENDS_SRC_DIR}/capstone)
set(CAPSTONE_INCLUDE ${CAPSTONE_SRC}/include/capstone)
set(CAPSTONE_BUILD_TESTS  OFF CACHE BOOL "")
set(CAPSTONE_BUILD_SHARED OFF CACHE BOOL "")
set(CAPSTONE_BUILD_STATIC ON  CACHE BOOL "")
add_subdirectory(${CAPSTONE_SRC})

### LibREDasm

# Support
file(GLOB_RECURSE SUPPORT_HEADERS core/support/*.h*)
file(GLOB_RECURSE SUPPORT_SOURCES core/support/*.cpp)

# Types
file(GLOB_RECURSE TYPES_HEADERS core/types/*.h*)
file(GLOB_RECURSE TYPES_SOURCES core/types/*.cpp)

# Plugins
file(GLOB_RECURSE PLUGINS_HEADERS core/plugins/*.h*)
file(GLOB_RECURSE PLUGINS_SOURCES core/plugins/*.cpp)

# Loaders
file(GLOB_RECURSE LOADER_HEADERS core/loaders/*.h*)
file(GLOB_RECURSE LOADER_SOURCES core/loaders/*.cpp)

# Assemblers
file(GLOB_RECURSE ASSEMBLER_HEADERS core/assemblers/*.h*)
file(GLOB_RECURSE ASSEMBLER_SOURCES core/assemblers/*.cpp)

# Analyzers
file(GLOB_RECURSE ANALYZER_HEADERS core/analyzer/*.h*)
file(GLOB_RECURSE ANALYZER_SOURCES core/analyzer/*.cpp)

# Emulators
file(GLOB_RECURSE EMULATOR_HEADERS core/emulator/*.h*)
file(GLOB_RECURSE EMULATOR_SOURCES core/emulator/*.cpp)

# Disassembler
file(GLOB_RECURSE DISASSEMBLER_HEADERS core/disassembler/*.h)
file(GLOB_RECURSE DISASSEMBLER_SOURCES core/disassembler/*.cpp)

# Database
file(GLOB_RECURSE DATABASE_HEADERS core/database/*.h)
file(GLOB_RECURSE DATABASE_SOURCES core/database/*.cpp)

# Graph
file(GLOB_RECURSE GRAPH_HEADERS core/graph/*.h)
file(GLOB_RECURSE GRAPH_SOURCES core/graph/*.cpp)

SET(SOURCES
    ${MINIZ_SOURCES}
    ${BINUTILS_DEMANGLER_SOURCES}
    ${SUPPORT_SOURCES}
    ${TYPES_SOURCES}
    ${PLUGINS_SOURCES}
    ${LOADER_SOURCES}
    ${ASSEMBLER_SOURCES}
    ${ANALYZER_SOURCES}
    ${EMULATOR_SOURCES}
    ${DISASSEMBLER_SOURCES}
    ${DATABASE_SOURCES}
    ${GRAPH_SOURCES}
    core/redasm_api.cpp
    core/redasm_ui.cpp
    core/redasm_context.cpp)

set(HEADERS
    ${MINIZ_HEADERS}
    ${BINUTILS_DEMANGLER_HEADERS}
    ${SUPPORT_HEADERS}
    ${TYPES_HEADERS}
    ${PLUGINS_HEADERS}
    ${LOADER_HEADERS}
    ${ASSEMBLER_HEADERS}
    ${ANALYZER_HEADERS}
    ${EMULATOR_HEADERS}
    ${DISASSEMBLER_HEADERS}
    ${DATABASE_HEADERS}
    ${GRAPH_HEADERS}
    core/redasm.h
    core/redasm_macros.h
    core/redasm_api.h
    core/redasm_ui.h
    core/redasm_context.h)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
add_dependencies(${PROJECT_NAME} capstone-static)

if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
                           ${CAPSTONE_INCLUDE}
                           ${DEPENDS_SINGLE_HEADERS}
                           ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} capstone-static)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(${PROJECT_NAME} pthread)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "LibREDasm")
