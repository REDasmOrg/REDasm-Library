cmake_minimum_required(VERSION 3.10)

project(LibREDasm)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(DEPENDS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/depends)
set(DEPENDS_BUILD_DIR ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/depends)
set(DEPENDS_HEADERS_DIR ${DEPENDS_SRC_DIR}/include)

### Dependencies

# Single Headers
set(DEPENDS_SINGLE_HEADERS ${DEPENDS_HEADERS_DIR}
                           ${DEPENDS_HEADERS_DIR}/json
                           ${DEPENDS_HEADERS_DIR}/visit_struct
                           ${DEPENDS_HEADERS_DIR}/binutils_demangler
                           ${DEPENDS_SRC_DIR}/miniz)

# MiniZ
set(MINIZ_HEADERS ${DEPENDS_SRC_DIR}/miniz/miniz.h)
set(MINIZ_SOURCES ${DEPENDS_SRC_DIR}/miniz/miniz.c)

# Binutils Demangler (libiberty)
set(BINUTILS_DEMANGLER_DIR ${DEPENDS_HEADERS_DIR}/binutils_demangler)
set(BINUTILS_DEMANGLER_HEADERS ${BINUTILS_DEMANGLER_DIR}/demangle.h ${BINUTILS_DEMANGLER_DIR}/cp-demangle.h)
set(BINUTILS_DEMANGLER_SOURCES ${BINUTILS_DEMANGLER_DIR}/cp-demangle.c)

# Capstone (Override Settings)
set(CAPSTONE_SRC ${DEPENDS_SRC_DIR}/capstone)
set(CAPSTONE_INCLUDE ${CAPSTONE_SRC}/include/capstone)
set(CAPSTONE_BUILD_TESTS  OFF CACHE BOOL "")
set(CAPSTONE_BUILD_SHARED OFF CACHE BOOL "")
set(CAPSTONE_BUILD_STATIC ON  CACHE BOOL "")
add_subdirectory(${CAPSTONE_SRC})

### LibREDasm

# Support
file(GLOB_RECURSE SUPPORT_HEADERS redasm/support/*.h)
file(GLOB_RECURSE SUPPORT_SOURCES redasm/support/*.cpp)

# Types
file(GLOB_RECURSE TYPES_HEADERS redasm/types/*.h)
file(GLOB_RECURSE TYPES_SOURCES redasm/types/*.cpp)

# Plugin
file(GLOB_RECURSE PLUGINS_HEADERS redasm/plugins/*.h)
file(GLOB_RECURSE PLUGINS_SOURCES redasm/plugins/*.cpp)

# Format
file(GLOB_RECURSE FORMAT_HEADERS redasm/formats/*.h)
file(GLOB_RECURSE FORMAT_SOURCES redasm/formats/*.cpp)

# Assembler
file(GLOB_RECURSE ASSEMBLER_HEADERS redasm/assemblers/*.h)
file(GLOB_RECURSE ASSEMBLER_SOURCES redasm/assemblers/*.cpp)

# Analyzer
file(GLOB_RECURSE ANALYZER_HEADERS redasm/analyzer/*.h)
file(GLOB_RECURSE ANALYZER_SOURCES redasm/analyzer/*.cpp)

# Emulator
file(GLOB_RECURSE EMULATOR_HEADERS redasm/emulator/*.h)
file(GLOB_RECURSE EMULATOR_SOURCES redasm/emulator/*.cpp)

# Disassembler
file(GLOB_RECURSE DISASSEMBLER_HEADERS redasm/disassembler/*.h)
file(GLOB_RECURSE DISASSEMBLER_SOURCES redasm/disassembler/*.cpp)

# Database
file(GLOB_RECURSE DATABASE_HEADERS redasm/database/*.h)
file(GLOB_RECURSE DATABASE_SOURCES redasm/database/*.cpp)

# Graph
file(GLOB_RECURSE GRAPH_HEADERS redasm/graph/*.h)
file(GLOB_RECURSE GRAPH_SOURCES redasm/graph/*.cpp)

SET(SOURCES
    ${MINIZ_SOURCES}
    ${BINUTILS_DEMANGLER_SOURCES}
    ${SUPPORT_SOURCES}
    ${TYPES_SOURCES}
    ${PLUGINS_SOURCES}
    ${FORMAT_SOURCES}
    ${ASSEMBLER_SOURCES}
    ${ANALYZER_SOURCES}
    ${EMULATOR_SOURCES}
    ${DISASSEMBLER_SOURCES}
    ${DATABASE_SOURCES}
    ${GRAPH_SOURCES}
    redasm/redasm_api.cpp
    redasm/redasm_runtime.cpp)

set(HEADERS
    ${MINIZ_HEADERS}
    ${BINUTILS_DEMANGLER_HEADERS}
    ${SUPPORT_HEADERS}
    ${TYPES_HEADERS}
    ${PLUGINS_HEADERS}
    ${FORMAT_HEADERS}
    ${ASSEMBLER_HEADERS}
    ${ANALYZER_HEADERS}
    ${EMULATOR_HEADERS}
    ${DISASSEMBLER_HEADERS}
    ${DATABASE_HEADERS}
    ${GRAPH_HEADERS}
    redasm/redasm.h
    redasm/redasm_api.h
    redasm/redasm_runtime.h)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
add_dependencies(${PROJECT_NAME} capstone-static)

if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
                           ${CAPSTONE_INCLUDE}
                           ${DEPENDS_SINGLE_HEADERS}
                           ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} capstone-static)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(${PROJECT_NAME} pthread)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "LibREDasm")
