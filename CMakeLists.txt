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
set(DEPENDS_SINGLE_HEADERS ${DEPENDS_HEADERS_DIR}/json)

#ZLib
set(ZLIB_BUILD ${DEPENDS_BUILD_DIR}/zlib)
set(ZLIB_INCLUDE ${DEPENDS_SRC_DIR}/zlib)
add_subdirectory(${DEPENDS_SRC_DIR}/zlib)

# Capstone (Override Settings)
set(CAPSTONE_SRC ${DEPENDS_SRC_DIR}/capstone)
set(CAPSTONE_INCLUDE ${CAPSTONE_SRC}/include/capstone)
set(CAPSTONE_BUILD_TESTS  OFF CACHE BOOL "")
set(CAPSTONE_BUILD_SHARED OFF CACHE BOOL "")
set(CAPSTONE_BUILD_STATIC ON  CACHE BOOL "")
add_subdirectory(${CAPSTONE_SRC})

# Formats & Assemblers
file(GLOB_RECURSE ASSEMBLER_HEADERS redasm/assemblers/*.h)
file(GLOB_RECURSE ASSEMBLER_SOURCES redasm/assemblers/*.cpp)
file(GLOB_RECURSE FORMAT_HEADERS redasm/formats/*.h)
file(GLOB_RECURSE FORMAT_SOURCES redasm/formats/*.cpp)

# LibREDasm
SET(SOURCES
    ${FORMAT_SOURCES}
    ${ASSEMBLER_SOURCES}
    redasm/plugins/plugins.cpp
    redasm/plugins/format.cpp
    redasm/analyzer/analyzer.cpp
    redasm/disassembler/disassembler.cpp
    redasm/plugins/assembler/printer.cpp
    redasm/support/utils.cpp
    redasm/support/demangler.cpp
    redasm/support/rtti/msvc/rtti_msvc.cpp
    redasm/disassembler/disassemblerbase.cpp
    redasm/disassembler/types/referencetable.cpp
    redasm/disassembler/types/symboltable.cpp
    redasm/support/coff/coff_symboltable.cpp
    redasm/support/serializer.cpp
    redasm/disassembler/disassemblerapi.cpp
    redasm/disassembler/graph/functiongraph.cpp
    redasm/support/ordinals.cpp
    redasm/plugins/assembler/assembler.cpp
    redasm/disassembler/listing/listingdocument.cpp
    redasm/disassembler/listing/listingrenderer.cpp
    redasm/support/concurrent/jobspool.cpp
    redasm/support/concurrent/job.cpp
    redasm/disassembler/listing/listingcursor.cpp
    redasm/graph/graph.cpp
    redasm/plugins/assembler/algorithm/algorithm.cpp
    redasm/plugins/assembler/algorithm/controlflow.cpp
    redasm/plugins/assembler/algorithm/linearsweep.cpp
    redasm/plugins/assembler/algorithm/statemachine.cpp
    redasm/plugins/emulator.cpp
    redasm/types/buffer/abstractbuffer.cpp
    redasm/types/buffer/memorybuffer.cpp
    redasm/types/buffer/bufferview.cpp
    redasm/redasm_api.cpp
    redasm/redasm_runtime.cpp
    redasm/database/database.cpp
    redasm/database/signaturedb.cpp
    redasm/support/compression.cpp
    redasm/disassembler/listing/instructioncache.cpp)

set(HEADERS
    ${FORMAT_HEADERS}
    ${ASSEMBLER_HEADERS}
    redasm/redasm.h
    redasm/plugins/format.h
    redasm/plugins/base.h
    redasm/plugins/plugins.h
    redasm/analyzer/analyzer.h
    redasm/disassembler/disassembler.h
    redasm/plugins/assembler/printer.h
    redasm/support/demangler.h
    redasm/support/utils.h
    redasm/support/rtti/msvc/rtti_msvc.h
    redasm/disassembler/disassemblerbase.h
    redasm/disassembler/types/referencetable.h
    redasm/disassembler/types/symboltable.h
    redasm/support/coff/coff_symboltable.h
    redasm/support/coff/coff_types.h
    redasm/support/coff/coff_constants.h
    redasm/support/cachemap.h
    redasm/support/hash.h
    redasm/support/safe_ptr.h
    redasm/support/serializer.h
    redasm/disassembler/disassemblerapi.h
    redasm/disassembler/graph/functiongraph.h
    redasm/support/ordinals.h
    redasm/plugins/assembler/assembler.h
    redasm/disassembler/listing/listingdocument.h
    redasm/disassembler/listing/listingrenderer.h
    redasm/support/concurrent/jobspool.h
    redasm/support/concurrent/job.h
    redasm/support/event.h
    redasm/disassembler/listing/listingcursor.h
    redasm/graph/graph.h
    redasm/support/dispatcher.h
    redasm/plugins/assembler/algorithm/algorithm.h
    redasm/plugins/assembler/algorithm/controlflow.h
    redasm/plugins/assembler/algorithm/linearsweep.h
    redasm/plugins/assembler/algorithm/statemachine.h
    redasm/emulator/emulator_alu.h
    redasm/emulator/emulator_base.h
    redasm/plugins/emulator.h
    redasm/emulator/emulator_type.h
    redasm/redasm_api.h
    redasm/redasm_runtime.h
    redasm/database/database.h
    redasm/database/signaturedb.h
    redasm/support/compression.h
    redasm/types/buffer/abstractbuffer.h
    redasm/types/buffer/memorybuffer.h
    redasm/types/buffer/bufferview.h
    redasm/types/numeric_type.h
    redasm/types/base_types.h
    redasm/types/endianness/endianness.h
    redasm/types/endianness/endianness_base.h
    redasm/disassembler/listing/instructioncache.h)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
add_dependencies(${PROJECT_NAME} capstone-static zlibstatic)

if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
                           ${ZLIB_BUILD}
                           ${ZLIB_INCLUDE}
                           ${CAPSTONE_INCLUDE}
                           ${DEPENDS_SINGLE_HEADERS}
                           ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} capstone-static zlibstatic)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(${PROJECT_NAME} pthread)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "LibREDasm")
