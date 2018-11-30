cmake_minimum_required(VERSION 3.10)

project(LibREDasm)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(DEPENDS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/depends)
set(DEPENDS_BUILD_DIR ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/depends)
set(DEPENDS_SINGLE_DIR ${DEPENDS_SRC_DIR}/include)

### Dependencies
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

set(HEADERS
    redasm/redasm.h
    redasm/plugins/format.h
    redasm/plugins/base.h
    redasm/plugins/plugins.h
    redasm/analyzer/analyzer.h
    redasm/disassembler/disassembler.h
    redasm/formats/psxexe/psxexe.h
    redasm/formats/psxexe/psxexe_analyzer.h
    redasm/formats/pe/pe.h
    redasm/formats/pe/pe_constants.h
    redasm/formats/pe/pe_headers.h
    redasm/plugins/assembler/printer.h
    redasm/formats/pe/pe_analyzer.h
    redasm/formats/pe/pe_utils.h
    redasm/formats/elf/elf.h
    redasm/formats/elf/elf64_header.h
    redasm/formats/elf/elf32_header.h
    redasm/formats/elf/elf_common.h
    redasm/support/demangler.h
    redasm/support/utils.h
    redasm/assemblers/mips/mips.h
    redasm/assemblers/x86/x86.h
    redasm/formats/pe/vb/vb_analyzer.h
    redasm/formats/pe/vb/vb_header.h
    redasm/formats/pe/vb/vb_components.h
    redasm/formats/pe/pe_imports.h
    redasm/disassembler/disassemblerbase.h
    redasm/disassembler/types/referencetable.h
    redasm/disassembler/types/symboltable.h
    redasm/support/coff/coff_symboltable.h
    redasm/support/coff/coff_types.h
    redasm/support/coff/coff_constants.h
    redasm/support/cachemap.h
    redasm/support/hash.h
    redasm/signatures/patparser.h
    redasm/support/serializer.h
    redasm/signatures/signaturedb.h
    redasm/formats/pe/pe_resources.h
    redasm/formats/pe/borland/borland_types.h
    redasm/formats/pe/borland/borland_version.h
    redasm/formats/binary/binary.h
    redasm/assemblers/chip8/chip8.h
    redasm/assemblers/mips/mips_printer.h
    redasm/assemblers/mips/mips_quirks.h
    redasm/assemblers/x86/x86_printer.h
    redasm/assemblers/chip8/chip8_printer.h
    redasm/assemblers/chip8/chip8_registers.h
    redasm/assemblers/mips/mips_emulator.h
    redasm/formats/dex/dex.h
    redasm/formats/dex/dex_constants.h
    redasm/formats/dex/dex_header.h
    redasm/assemblers/dalvik/dalvik.h
    redasm/assemblers/dalvik/dalvik_printer.h
    redasm/assemblers/dalvik/dalvik_metadata.h
    redasm/assemblers/dalvik/dalvik_opcodes.h
    redasm/formats/dex/dex_statemachine.h
    redasm/formats/dex/dex_utils.h
    redasm/formats/pe/pe_debug.h
    redasm/formats/pe/pe_common.h
    redasm/formats/elf/elf_analyzer.h
    redasm/disassembler/disassemblerapi.h
    redasm/formats/pe/dotnet/dotnet_header.h
    redasm/assemblers/cil/cil.h
    redasm/formats/pe/dotnet/dotnet.h
    redasm/formats/pe/dotnet/dotnet_tables.h
    redasm/formats/pe/dotnet/dotnet_reader.h
    redasm/disassembler/graph/functiongraph.h
    redasm/formats/xbe/xbe.h
    redasm/formats/xbe/xbe_header.h
    redasm/support/ordinals.h
    redasm/formats/gba/gba.h
    redasm/formats/gba/gba_analyzer.h
    redasm/assemblers/metaarm/metaarm.h
    redasm/assemblers/metaarm/metaarm_printer.h
    redasm/assemblers/metaarm/metaarm_emulator.h
    redasm/assemblers/metaarm/arm.h
    redasm/assemblers/metaarm/arm_common.h
    redasm/plugins/assembler/assembler.h
    redasm/disassembler/listing/listingdocument.h
    redasm/disassembler/listing/listingrenderer.h
    redasm/support/timer.h
    redasm/support/event.h
    redasm/disassembler/listing/listingcursor.h
    redasm/graph/graph.h
    redasm/support/dispatcher.h
    redasm/assemblers/metaarm/arm_thumb.h
    redasm/assemblers/dalvik/dalvik_algorithm.h
    redasm/plugins/assembler/algorithm/algorithm.h
    redasm/plugins/assembler/algorithm/controlflow.h
    redasm/plugins/assembler/algorithm/linearsweep.h
    redasm/plugins/assembler/algorithm/statemachine.h
    redasm/assemblers/metaarm/metaarm_algorithm.h
    redasm/emulator/emulator_alu.h
    redasm/emulator/emulator_base.h
    redasm/plugins/emulator.h
    redasm/emulator/emulator_type.h
    redasm/redasm_api.h
    redasm/redasm_runtime.h
    redasm/redasm_types.h
    redasm/redasm_buffer.h
    redasm/redasm_endianness.h
    redasm/database/database.h
    redasm/support/compression.h
    redasm/disassembler/listing/instructioncache.h )

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
                           ${ZLIB_BUILD}
                           ${ZLIB_INCLUDE}
                           ${CAPSTONE_INCLUDE}
                           ${DEPENDS_SINGLE_DIR}
                           ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} capstone-static zlibstatic)
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "LibREDasm")
