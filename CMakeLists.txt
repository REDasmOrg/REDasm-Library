cmake_minimum_required(VERSION 2.8)

project(REDasm)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(DEPENDS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/depends)
set(DEPENDS_SINGLE_DIR ${DEPENDS_DIR}/include)

### Dependencies
#ZLib
set(ZLIB_BUILD ${CMAKE_BINARY_DIR}/depends/zlib)
add_subdirectory(${DEPENDS_DIR}/zlib)

# Capstone
set(CAPSTONE_SRC ${DEPENDS_DIR}/capstone)
set(CAPSTONE_BUILD_TESTS  OFF CACHE BOOL "")
set(CAPSTONE_BUILD_SHARED OFF CACHE BOOL "")
set(CAPSTONE_BUILD_STATIC ON  CACHE BOOL "")
add_subdirectory(${CAPSTONE_SRC})

# LibREDasm
SET(SOURCES
    redasm/plugins/plugins.cpp
    redasm/plugins/format.cpp
    redasm/analyzer/analyzer.cpp
    redasm/disassembler/disassembler.cpp
    redasm/formats/psxexe/psxexe.cpp
    redasm/formats/psxexe/psxexe_analyzer.cpp
    redasm/formats/pe/pe.cpp
    redasm/plugins/assembler/printer.cpp
    redasm/formats/pe/pe_analyzer.cpp
    redasm/formats/pe/pe_utils.cpp
    redasm/formats/elf/elf.cpp
    redasm/support/utils.cpp
    redasm/support/demangler.cpp
    redasm/formats/pe/vb/vb_analyzer.cpp
    redasm/formats/pe/vb/vb_components.cpp
    redasm/formats/pe/pe_imports.cpp
    redasm/disassembler/disassemblerbase.cpp
    redasm/disassembler/types/referencetable.cpp
    redasm/disassembler/types/symboltable.cpp
    redasm/support/coff/coff_symboltable.cpp
    redasm/support/hash.cpp
    redasm/signatures/patparser.cpp
    redasm/support/serializer.cpp
    redasm/signatures/signaturedb.cpp
    redasm/redasm.cpp
    redasm/formats/pe/pe_resources.cpp
    redasm/formats/pe/borland/borland_version.cpp
    redasm/formats/binary/binary.cpp
    redasm/assemblers/chip8/chip8.cpp
    redasm/assemblers/mips/mips_quirks.cpp
    redasm/assemblers/mips/mips_printer.cpp
    redasm/assemblers/x86/x86_printer.cpp
    redasm/assemblers/chip8/chip8_printer.cpp
    redasm/assemblers/mips/mips_emulator.cpp
    redasm/formats/dex/dex.cpp
    redasm/assemblers/dalvik/dalvik.cpp
    redasm/assemblers/dalvik/dalvik_printer.cpp
    redasm/formats/dex/dex_statemachine.cpp
    redasm/formats/dex/dex_utils.cpp
    redasm/formats/elf/elf_analyzer.cpp
    redasm/disassembler/disassemblerapi.cpp
    redasm/assemblers/cil/cil.cpp
    redasm/formats/pe/dotnet/dotnet.cpp
    redasm/formats/pe/dotnet/dotnet_reader.cpp
    redasm/disassembler/graph/functiongraph.cpp
    redasm/formats/xbe/xbe.cpp
    redasm/support/ordinals.cpp
    redasm/formats/gba/gba.cpp
    redasm/formats/gba/gba_analyzer.cpp
    redasm/assemblers/metaarm/metaarm.cpp
    redasm/assemblers/metaarm/metaarm_printer.cpp
    redasm/assemblers/metaarm/metaarm_emulator.cpp
    redasm/assemblers/metaarm/arm.cpp
    redasm/plugins/assembler/assembler.cpp
    redasm/disassembler/listing/listingdocument.cpp
    redasm/disassembler/listing/listingrenderer.cpp
    redasm/support/timer.cpp
    redasm/disassembler/listing/listingcursor.cpp
    redasm/graph/graph.cpp
    redasm/assemblers/metaarm/arm_thumb.cpp
    redasm/assemblers/dalvik/dalvik_algorithm.cpp
    redasm/plugins/assembler/algorithm/algorithm.cpp
    redasm/plugins/assembler/algorithm/controlflow.cpp
    redasm/plugins/assembler/algorithm/linearsweep.cpp
    redasm/plugins/assembler/algorithm/statemachine.cpp
    redasm/assemblers/metaarm/metaarm_algorithm.cpp
    redasm/plugins/emulator.cpp
    redasm/redasm_api.cpp
    redasm/redasm_runtime.cpp
    redasm/redasm_buffer.cpp
    redasm/redasm_endianness.cpp
    redasm/database/database.cpp
    redasm/support/compression.cpp
    redasm/disassembler/listing/instructioncache.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_compile_features(REDasm PRIVATE cxx_std_11)
target_include_directories(REDasm PUBLIC ${CMAKE_SOURCE_DIR} ${CAPSTONE_SRC}/include/capstone ${ZLIB_BUILD} ${DEPENDS_SINGLE_DIR})
target_link_libraries(REDasm capstone-static zlib)
