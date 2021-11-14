/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2021 Philippe Steinmann.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "ElfProgramHeaderTestUtils.h"
#include "ElfSectionHeaderTestUtils.h"
#include "ElfFileIoTestUtils.h"
#include "Mdt/DeployUtils/Impl/Elf/FileAllHeaders.h"
#include "Mdt/DeployUtils/Impl/Elf/SectionSegmentUtils.h"
#include <cstdint>
#include <cassert>

struct TestHeadersSetup
{
  uint64_t programHeaderTableOffset = 0;
  uint64_t sectionHeaderTableOffset = 0;
  uint64_t programInterpreterSectionOffset = 0;
  uint64_t programInterpreterSectionAddress = 0;
  uint64_t programInterpreterSectionSize = 0;
  uint64_t noteAbiTagSectionOffset = 0;
//   uint64_t noteAbiTagSectionOffset = 50;
  uint64_t noteAbiTagSectionAddress = 50;
  uint64_t noteAbiTagSectionSize = 10;
  uint64_t noteGnuBuilIdSectionOffset = 60;
  uint64_t noteGnuBuilIdSectionAddress = 60;
  uint64_t noteGnuBuilIdSectionSize = 10;
  uint64_t dynamicSectionOffset = 0;
  uint64_t dynamicSectionSize = 0;
  uint64_t dynamicSectionAddress = 0;
  uint64_t dynamicSectionAlignment = 0;
  uint64_t dynamicStringTableOffset = 0;
  uint64_t dynamicStringTableSize = 0;
  uint64_t dynamicStringTableAddress = 0;
  uint64_t sectionNameStringTableOffset = 0;

  bool containsProgramInterpreter() const noexcept
  {
    if(programInterpreterSectionOffset == 0){
      return false;
    }
    if(programInterpreterSectionAddress == 0){
      return false;
    }
    if(programInterpreterSectionSize == 0){
      return false;
    }
    return true;
  }

  bool containsDynamicStringTable() const noexcept
  {
    if(dynamicStringTableOffset == 0){
      return false;
    }
    if(dynamicStringTableAddress == 0){
      return false;
    }
    if(dynamicStringTableSize == 0){
      return false;
    }
    return true;
  }
};

// inline
// void makeAndAddProgramInterpreterToTables(const TestHeadersSetup & setup,
//                                           Mdt::DeployUtils::Impl::Elf::ProgramHeaderTable & programHeaderTable,
//                                           std::vector<Mdt::DeployUtils::Impl::Elf::SectionHeader> & sectionHeaderTable)
// {
//   using Mdt::DeployUtils::Impl::Elf::ProgramHeader;
//   using Mdt::DeployUtils::Impl::Elf::SectionHeader;
// 
//   assert( setup.containsProgramInterpreter() );
// 
//   ProgramHeader programInterpreterSectionProgramHeader = makeProgramInterpreterProgramHeader();
//   programInterpreterSectionProgramHeader.offset = setup.programInterpreterSectionOffset;
//   programInterpreterSectionProgramHeader.filesz = setup.programInterpreterSectionSize;
//   programInterpreterSectionProgramHeader.vaddr = setup.programInterpreterSectionAddress;
//   programInterpreterSectionProgramHeader.align = 1;
//   programInterpreterSectionProgramHeader.paddr = setup.programInterpreterSectionAddress;
//   programInterpreterSectionProgramHeader.memsz = setup.programInterpreterSectionSize;
// 
//   SectionHeader programInterpreterSectionHeader = makeProgramInterpreterSectionHeader();
//   programInterpreterSectionHeader.name = ".interp";
//   programInterpreterSectionHeader.offset = setup.programInterpreterSectionOffset;
//   programInterpreterSectionHeader.size = setup.programInterpreterSectionSize;
//   programInterpreterSectionHeader.addr = setup.programInterpreterSectionAddress;
//   programInterpreterSectionHeader.addralign = 1;
// 
//   programHeaderTable.addHeaderFromFile(programInterpreterSectionProgramHeader);
//   sectionHeaderTable.push_back(programInterpreterSectionHeader);
// }

inline
Mdt::DeployUtils::Impl::Elf::ProgramHeader makeProgramInterpreterProgramHeader(const TestHeadersSetup & setup)
{
  using Mdt::DeployUtils::Impl::Elf::ProgramHeader;

  assert( setup.containsProgramInterpreter() );

  ProgramHeader header = makeProgramInterpreterProgramHeader();
  header.offset = setup.programInterpreterSectionOffset;
  header.filesz = setup.programInterpreterSectionSize;
  header.vaddr = setup.programInterpreterSectionAddress;
  header.align = 1;
  header.paddr = setup.programInterpreterSectionAddress;
  header.memsz = setup.programInterpreterSectionSize;

  return header;
}

inline
Mdt::DeployUtils::Impl::Elf::SectionHeader makeProgramInterpreterSectionHeader(const TestHeadersSetup & setup)
{
  using Mdt::DeployUtils::Impl::Elf::SectionHeader;

  assert( setup.containsProgramInterpreter() );

  SectionHeader header = makeProgramInterpreterSectionHeader();
  header.name = ".interp";
  header.offset = setup.programInterpreterSectionOffset;
  header.size = setup.programInterpreterSectionSize;
  header.addr = setup.programInterpreterSectionAddress;
  header.addralign = 1;

  return header;
}

inline
Mdt::DeployUtils::Impl::Elf::FileAllHeaders makeTestHeaders(const TestHeadersSetup & setup)
{
  using Mdt::DeployUtils::Impl::Elf::MoveSectionAlignment;
  using Mdt::DeployUtils::Impl::Elf::FileHeader;
  using Mdt::DeployUtils::Impl::Elf::ProgramHeader;
  using Mdt::DeployUtils::Impl::Elf::SegmentType;
  using Mdt::DeployUtils::Impl::Elf::ProgramHeaderTable;
  using Mdt::DeployUtils::Impl::Elf::SectionHeader;
  using Mdt::DeployUtils::Impl::Elf::FileAllHeaders;
  using Mdt::DeployUtils::Impl::Elf::makeNoteProgramHeaderCoveringSections;

  FileAllHeaders headers;
  FileHeader fileHeader = make64BitLittleEndianFileHeader();
  fileHeader.phoff = setup.programHeaderTableOffset;
  fileHeader.shoff = setup.sectionHeaderTableOffset;

//   ProgramHeaderTable programHeaderTable;
//   std::vector<SectionHeader> sectionHeaderTable;

//   if( setup.containsProgramInterpreter() ){
//     makeAndAddProgramInterpreterToTables(setup, programHeaderTable, sectionHeaderTable);
//   }
//   ProgramHeader programInterpreterSectionProgramHeader = makeProgramInterpreterProgramHeader();
//   programInterpreterSectionProgramHeader.offset = setup.programInterpreterSectionOffset;
//   programInterpreterSectionProgramHeader.filesz = setup.programInterpreterSectionSize;
//   programInterpreterSectionProgramHeader.vaddr = setup.programInterpreterSectionAddress;
//   programInterpreterSectionProgramHeader.align = 1;
//   programInterpreterSectionProgramHeader.paddr = setup.programInterpreterSectionAddress;
//   programInterpreterSectionProgramHeader.memsz = setup.programInterpreterSectionSize;
// 
//   SectionHeader programInterpreterSectionHeader = makeProgramInterpreterSectionHeader();
//   programInterpreterSectionHeader.name = ".interp";
//   programInterpreterSectionHeader.offset = setup.programInterpreterSectionOffset;
//   programInterpreterSectionHeader.size = setup.programInterpreterSectionSize;
//   programInterpreterSectionHeader.addr = setup.programInterpreterSectionAddress;
//   programInterpreterSectionHeader.addralign = 1;

//   const ProgramHeader programInterpreterSectionProgramHeader = makeProgramInterpreterProgramHeader(setup);
//   const SectionHeader programInterpreterSectionHeader = makeProgramInterpreterSectionHeader(setup);

  SectionHeader noteAbiTagSectionHeader = makeNoteSectionHeader(".note.ABI-tag");
  noteAbiTagSectionHeader.offset = setup.noteAbiTagSectionOffset;
  noteAbiTagSectionHeader.size = setup.noteAbiTagSectionSize;
  noteAbiTagSectionHeader.addr = setup.noteAbiTagSectionAddress;
  noteAbiTagSectionHeader.addralign = 4;

  SectionHeader noteGnuBuildIdSectionHeader = makeNoteSectionHeader(".note.gnu.build-id");
  noteGnuBuildIdSectionHeader.offset = setup.noteGnuBuilIdSectionOffset;
  noteGnuBuildIdSectionHeader.size = setup.noteGnuBuilIdSectionSize;
  noteGnuBuildIdSectionHeader.addr = setup.noteGnuBuilIdSectionAddress;
  noteGnuBuildIdSectionHeader.addralign = 4;

  ProgramHeader noteProgramHeader = makeNoteProgramHeaderCoveringSections({noteAbiTagSectionHeader, noteGnuBuildIdSectionHeader});

  ProgramHeader dynamicSectionProgramHeader = makeDynamicSectionProgramHeader();
  dynamicSectionProgramHeader.offset = setup.dynamicSectionOffset;
  dynamicSectionProgramHeader.filesz = setup.dynamicSectionSize;
  dynamicSectionProgramHeader.vaddr = setup.dynamicSectionAddress;
  dynamicSectionProgramHeader.align = setup.dynamicSectionAlignment;
  dynamicSectionProgramHeader.paddr = setup.dynamicSectionAddress;
  dynamicSectionProgramHeader.memsz = setup.dynamicSectionSize;

  SectionHeader dynamicSectionHeader = makeDynamicSectionHeader();
  dynamicSectionHeader.name = ".dynamic";
  dynamicSectionHeader.offset = setup.dynamicSectionOffset;
  dynamicSectionHeader.size = setup.dynamicSectionSize;
  dynamicSectionHeader.addr = setup.dynamicSectionAddress;
  dynamicSectionHeader.addralign = setup.dynamicSectionAlignment;
  dynamicSectionHeader.link = 5;

  SectionHeader dynamicStringTableSectionHeader = makeStringTableSectionHeader();
  dynamicStringTableSectionHeader.name = ".dynstr";
  dynamicStringTableSectionHeader.offset = setup.dynamicStringTableOffset;
  dynamicStringTableSectionHeader.size = setup.dynamicStringTableSize;
  dynamicStringTableSectionHeader.addr = setup.dynamicStringTableAddress;
  dynamicStringTableSectionHeader.addralign = 1;

  SectionHeader sectionNameStringTableHeader = makeStringTableSectionHeader();
  sectionNameStringTableHeader.name = "shstrtab";
  sectionNameStringTableHeader.offset = setup.sectionNameStringTableOffset;
  sectionNameStringTableHeader.size = 100;

  ProgramHeader programHeaderTableProgramHeader = makeProgramHeaderTableProgramHeader();
  programHeaderTableProgramHeader.offset = setup.programHeaderTableOffset;
  programHeaderTableProgramHeader.filesz = 2*56;
  programHeaderTableProgramHeader.vaddr = setup.programHeaderTableOffset;
  programHeaderTableProgramHeader.memsz = 2*56;

  ProgramHeaderTable programHeaderTable;
  programHeaderTable.addHeaderFromFile(programHeaderTableProgramHeader);
  if( setup.containsProgramInterpreter() ){
    programHeaderTable.addHeaderFromFile( makeProgramInterpreterProgramHeader(setup) );
  }
  programHeaderTable.addHeaderFromFile(noteProgramHeader);
  programHeaderTable.addHeaderFromFile(dynamicSectionProgramHeader);

  std::vector<SectionHeader> sectionHeaderTable;
  sectionHeaderTable.push_back( makeNullSectionHeader() );
  if( setup.containsProgramInterpreter() ){
    sectionHeaderTable.push_back( makeProgramInterpreterSectionHeader(setup) );
  }
  sectionHeaderTable.push_back(noteAbiTagSectionHeader);
  sectionHeaderTable.push_back(noteGnuBuildIdSectionHeader);
  if( setup.containsDynamicStringTable() ){
    dynamicSectionHeader.link = sectionHeaderTable.size()+1;
  }else{
    dynamicSectionHeader.link = 0;
  }
  sectionHeaderTable.push_back(dynamicSectionHeader);
  sectionHeaderTable.push_back(dynamicStringTableSectionHeader);
  sectionHeaderTable.push_back(sectionNameStringTableHeader);

  fileHeader.shstrndx = 6;

  headers.setFileHeader(fileHeader);
  headers.setProgramHeaderTable(programHeaderTable);
  headers.setSectionHeaderTable(sectionHeaderTable);

  return headers;
}
