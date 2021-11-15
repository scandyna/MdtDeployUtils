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
#include "catch2/catch.hpp"
#include "Catch2QString.h"
#include "TestFileUtils.h"
#include "ElfFileIoTestUtils.h"
#include "ElfFileAllHeadersImplTestUtils.h"
#include "ElfDynamicSectionImplTestCommon.h"
#include "ElfProgramHeaderTestUtils.h"
#include "ElfSectionHeaderTestUtils.h"
#include "ElfSymbolTableTestUtils.h"
#include "Mdt/DeployUtils/Impl/Elf/FileWriterFile.h"
#include "Mdt/DeployUtils/Impl/Elf/FileOffsetChanges.h"
#include <QLatin1String>
#include <cassert>

using Mdt::DeployUtils::Impl::Elf::FileWriterFile;
using Mdt::DeployUtils::Impl::Elf::FileWriterFileLayout;
using Mdt::DeployUtils::Impl::Elf::FileAllHeaders;
using Mdt::DeployUtils::Impl::Elf::DynamicSection;
using Mdt::DeployUtils::Impl::Elf::FileHeader;
using Mdt::DeployUtils::Impl::Elf::ProgramHeader;
using Mdt::DeployUtils::Impl::Elf::ProgramHeaderTable;
using Mdt::DeployUtils::Impl::Elf::SectionHeader;
using Mdt::DeployUtils::Impl::Elf::PartialSymbolTable;

FileAllHeaders makeBasicFileAllHeaders()
{
  FileAllHeaders headers;
  FileHeader fileHeader = make64BitLittleEndianFileHeader();
  fileHeader.phnum = 0;
  fileHeader.shnum = 0;
  headers.setFileHeader(fileHeader);

  return headers;
}

struct TestFileSetup
{
  uint64_t programHeaderTableOffset = 0;
  uint64_t sectionHeaderTableOffset = 0;
  uint64_t dynamicSectionOffset = 0;
  uint64_t dynamicSectionAddress = 0;
  uint64_t dynamicStringTableOffset = 0;
  uint64_t dynamicStringTableAddress = 0;
  int64_t dynSymOffset = 0;
  QString runPath;
  uint64_t sectionNameStringTableOffset = 0;

  uint64_t stringTableByteCount() const
  {
    if( runPath.isEmpty() ){
      return 1;
    }
    return 1 + static_cast<uint64_t>( runPath.size() ) + 1;
  }
};

FileWriterFile makeWriterFile_NEW(const TestFileSetup & setup)
{
  using Mdt::DeployUtils::Impl::Elf::Class;

  assert(setup.dynamicSectionOffset > 0);
  assert(setup.dynamicSectionAddress > 0);
  assert(setup.dynamicStringTableOffset > 0);
  assert(setup.dynamicStringTableAddress > 0);

  DynamicSection dynamicSection;
  dynamicSection.addEntry( makeNullEntry() );
  dynamicSection.addEntry( makeStringTableAddressEntry(setup.dynamicStringTableAddress) );
  dynamicSection.addEntry( makeStringTableSizeEntry(1) );
  dynamicSection.setRunPath(setup.runPath);

  TestHeadersSetup headersSetup;
  headersSetup.programHeaderTableOffset = setup.programHeaderTableOffset;
  headersSetup.sectionHeaderTableOffset = setup.sectionHeaderTableOffset;
  headersSetup.dynamicSectionOffset = setup.dynamicSectionOffset;
  headersSetup.dynamicSectionAddress = setup.dynamicSectionAddress;
  headersSetup.dynamicSectionSize = static_cast<uint64_t>( dynamicSection.byteCount(Class::Class64) );
  headersSetup.dynamicStringTableOffset = setup.dynamicStringTableOffset;
  headersSetup.dynamicStringTableAddress = setup.dynamicStringTableAddress;
  headersSetup.dynamicStringTableSize = static_cast<uint64_t>( dynamicSection.stringTable().byteCount() );
  headersSetup.sectionNameStringTableOffset = setup.sectionNameStringTableOffset;

  FileAllHeaders headers = makeTestHeaders(headersSetup);
  FileWriterFile file = FileWriterFile::fromOriginalFile(headers, dynamicSection);

  PartialSymbolTable symbolTable;
  int64_t dynSymEntryOffset = setup.dynSymOffset;
  PartialSymbolTableEntry dynamicSectionSymTabEntry = makeSectionAssociationSymbolTableEntryWithFileOffset(dynSymEntryOffset);
  dynamicSectionSymTabEntry.entry.shndx = headers.dynamicSectionHeaderIndex();
  dynSymEntryOffset += symbolTableEntrySize(headers.fileHeader().ident._class);
  PartialSymbolTableEntry dynStrSymTabEntry = makeSectionAssociationSymbolTableEntryWithFileOffset(dynSymEntryOffset);
  dynStrSymTabEntry.entry.shndx = headers.dynamicStringTableSectionHeaderIndex();
  symbolTable.addEntryFromFile(dynamicSectionSymTabEntry);
  symbolTable.addEntryFromFile(dynStrSymTabEntry);
  symbolTable.indexAssociationsKnownSections( headers.sectionHeaderTable() );

  file.setSectionSymbolTableFromFile(symbolTable);

  return file;
}

FileWriterFile makeWriterFile(const TestFileSetup & setup)
{
  return makeWriterFile_NEW(setup);

  using Mdt::DeployUtils::Impl::Elf::symbolTableEntrySize;

  FileHeader fileHeader = make64BitLittleEndianFileHeader();
  fileHeader.phoff = setup.programHeaderTableOffset;
  fileHeader.shoff = setup.sectionHeaderTableOffset;

  DynamicSection dynamicSection;
  dynamicSection.addEntry( makeNullEntry() );
  dynamicSection.addEntry( makeStringTableAddressEntry(setup.dynamicStringTableAddress) );
  dynamicSection.addEntry( makeStringTableSizeEntry(1) );
  dynamicSection.setRunPath(setup.runPath);

  const uint64_t dynamicSectionSize = dynamicSection.byteCount(fileHeader.ident._class);
  const uint64_t dynamicStringTableSize = dynamicSection.stringTable().byteCount();

  ProgramHeader dynamicSectionProgramHeader = makeDynamicSectionProgramHeader();
  dynamicSectionProgramHeader.offset = setup.dynamicSectionOffset;
  dynamicSectionProgramHeader.filesz = dynamicSectionSize;
  dynamicSectionProgramHeader.vaddr = setup.dynamicSectionAddress;
  dynamicSectionProgramHeader.paddr = setup.dynamicSectionAddress;
  dynamicSectionProgramHeader.memsz = dynamicSectionSize;
  dynamicSectionProgramHeader.align = 8;

  SectionHeader dynamicSectionHeader = makeDynamicSectionHeader();
  dynamicSectionHeader.name = ".dynamic";
  dynamicSectionHeader.offset = setup.dynamicSectionOffset;
  dynamicSectionHeader.size = dynamicSectionSize;
  dynamicSectionHeader.addr = setup.dynamicSectionAddress;
  dynamicSectionHeader.addralign = dynamicSectionProgramHeader.align;
  dynamicSectionHeader.link = 2;

  SectionHeader dynamicStringTableSectionHeader = makeStringTableSectionHeader();
  dynamicStringTableSectionHeader.name = ".dynstr";
  dynamicStringTableSectionHeader.offset = setup.dynamicStringTableOffset;
  dynamicStringTableSectionHeader.size = dynamicStringTableSize;
  dynamicStringTableSectionHeader.addr = setup.dynamicStringTableAddress;
  dynamicStringTableSectionHeader.addralign = 1;

  SectionHeader sectionNameStringTableHeader = makeStringTableSectionHeader();
  sectionNameStringTableHeader.name = ".shstrtab";
  sectionNameStringTableHeader.offset = setup.sectionNameStringTableOffset;
  sectionNameStringTableHeader.size = 10;

  ProgramHeader programHeaderTableProgramHeader = makeProgramHeaderTableProgramHeader();
  programHeaderTableProgramHeader.offset = setup.programHeaderTableOffset;
  programHeaderTableProgramHeader.filesz = 2*56;
  programHeaderTableProgramHeader.vaddr = setup.programHeaderTableOffset;
  programHeaderTableProgramHeader.paddr = setup.programHeaderTableOffset;
  programHeaderTableProgramHeader.memsz = 2*56;

  ProgramHeaderTable programHeaderTable;
  programHeaderTable.addHeaderFromFile(programHeaderTableProgramHeader);
  programHeaderTable.addHeaderFromFile(dynamicSectionProgramHeader);

  std::vector<SectionHeader> sectionHeaderTable;
  sectionHeaderTable.push_back( makeNullSectionHeader() );
  sectionHeaderTable.push_back(dynamicSectionHeader);
  sectionHeaderTable.push_back(dynamicStringTableSectionHeader);
  sectionHeaderTable.push_back(sectionNameStringTableHeader);

  PartialSymbolTable symbolTable;
  int64_t dynSymEntryOffset = setup.dynSymOffset;
  PartialSymbolTableEntry dynamicSectionSymTabEntry = makeSectionAssociationSymbolTableEntryWithFileOffset(dynSymEntryOffset);
  dynamicSectionSymTabEntry.entry.shndx = 1;
  dynSymEntryOffset += symbolTableEntrySize(fileHeader.ident._class);
  PartialSymbolTableEntry dynStrSymTabEntry = makeSectionAssociationSymbolTableEntryWithFileOffset(dynSymEntryOffset);
  dynStrSymTabEntry.entry.shndx = 2;
  symbolTable.addEntryFromFile(dynamicSectionSymTabEntry);
  symbolTable.addEntryFromFile(dynStrSymTabEntry);
  symbolTable.indexAssociationsKnownSections(sectionHeaderTable);

  fileHeader.shstrndx = 3;

  FileAllHeaders allHeaders;
  allHeaders.setFileHeader(fileHeader);
  allHeaders.setProgramHeaderTable(programHeaderTable);
  allHeaders.setSectionHeaderTable(sectionHeaderTable);

  FileWriterFile file = FileWriterFile::fromOriginalFile(allHeaders, dynamicSection);
  file.setSectionSymbolTableFromFile(symbolTable);

  return file;
}

FileWriterFileLayout makeFileLayoutFromFile(const FileWriterFile & file)
{
  return FileWriterFileLayout::fromFile( file.headers(), file.dynamicSection() );
}


TEST_CASE("FileWriterFileLayout")
{
  using Mdt::DeployUtils::Impl::Elf::Class;

  FileWriterFileLayout layout;

  SECTION("default constructed")
  {
    REQUIRE( layout.dynamicSectionOffset() == 0 );
    REQUIRE( layout.dynamicSectionSize() == 0 );
//     REQUIRE( layout.dynamicSectionEndOffset() == 0 );
    REQUIRE( layout.dynamicStringTableOffset() == 0 );
    REQUIRE( layout.dynamicStringTableSize() == 0 );
//     REQUIRE( layout.dynamicStringTableEndOffset() == 0 );
    REQUIRE( layout.globalOffsetRange().isEmpty() );
  }

  SECTION("from a file")
  {
    TestFileSetup setup;
    setup.programHeaderTableOffset = 50;
    setup.dynamicSectionOffset = 100;
    setup.dynamicSectionAddress = 100;
    setup.dynamicStringTableOffset = 1'000;
    setup.dynamicStringTableAddress = 1000;
    setup.sectionNameStringTableOffset = 5'000;
    setup.sectionHeaderTableOffset = 10'000;

    FileWriterFile file = makeWriterFile(setup);
    layout = makeFileLayoutFromFile(file);

    uint64_t expectedDynamicSectionSize = file.dynamicSection().byteCount(Class::Class64);
    int64_t expectedMinimumFileSize = file.fileHeader().minimumSizeToReadAllSectionHeaders();

    REQUIRE( layout.dynamicSectionOffset() == 100 );
    REQUIRE( layout.dynamicSectionSize() == expectedDynamicSectionSize );
//     REQUIRE( layout.dynamicSectionEndOffset() == 100 + expectedDynamicSectionSize -1 );
    REQUIRE( layout.dynamicStringTableOffset() == 1'000 );
    REQUIRE( layout.dynamicStringTableSize() == 1 );
//     REQUIRE( layout.dynamicStringTableEndOffset() == 1'000 );
    REQUIRE( layout.globalOffsetRange().minimumSizeToAccessRange() == expectedMinimumFileSize );
  }
}

TEST_CASE("fromOriginalFile")
{
  using Impl::Elf::SegmentType;
  using Impl::Elf::SectionType;

  TestFileSetup setup;
  setup.programHeaderTableOffset = 50;
  setup.dynamicStringTableOffset = 100;
  setup.dynamicStringTableAddress = 110;
  setup.dynamicSectionOffset = 1'000;
  setup.dynamicSectionAddress = 1'100;
  setup.sectionNameStringTableOffset = 5'000;
  setup.sectionHeaderTableOffset = 10'000;

  SECTION("No RUNPATH")
  {
    FileWriterFile file = makeWriterFile(setup);
    REQUIRE( file.fileHeader().seemsValid() );
    REQUIRE( file.fileHeader().phoff == 50 );
    REQUIRE( file.fileHeader().shoff == 10000 );
    REQUIRE( file.programHeaderTable().headerCount() == 2 );
    REQUIRE( file.programHeaderTable().headerAt(0).segmentType() == SegmentType::ProgramHeaderTable );
    REQUIRE( file.programHeaderTable().headerAt(0).offset == 50 );
    REQUIRE( file.programHeaderTable().headerAt(0).vaddr == 50 );
    REQUIRE( file.programHeaderTable().headerAt(0).paddr == 50 );
    REQUIRE( file.programHeaderTable().headerAt(1).segmentType() == SegmentType::Dynamic );
    REQUIRE( file.programHeaderTable().headerAt(1).offset == 1000 );
    REQUIRE( file.programHeaderTable().headerAt(1).vaddr == 1100 );
    REQUIRE( file.programHeaderTable().headerAt(1).paddr == 1100 );
    REQUIRE( file.sectionHeaderTable().size() == 4 );
    REQUIRE( file.sectionHeaderTable()[1].sectionType() == SectionType::Dynamic );
    REQUIRE( file.sectionHeaderTable()[1].offset == 1000 );
    REQUIRE( file.sectionHeaderTable()[1].addr == 1100 );
    REQUIRE( file.sectionHeaderTable()[2].sectionType() == SectionType::StringTable );
    REQUIRE( file.sectionHeaderTable()[2].offset == 100 );
    REQUIRE( file.sectionHeaderTable()[2].addr == 110 );
    REQUIRE( file.dynamicSectionOffset() == 1000 );
    REQUIRE( file.dynamicStringTableOffset() == 100 );
    REQUIRE( file.dynamicSection().getStringTableAddress() == 110 );
    REQUIRE( file.dynamicSection().getRunPath().isEmpty() );
    REQUIRE( file.headers().sectionNameStringTableHeader().offset == 5'000 );
  }

  SECTION("RUNPATH /tmp")
  {
    setup.runPath = QLatin1String("/tmp");
    FileWriterFile file = makeWriterFile(setup);
    REQUIRE( file.fileHeader().seemsValid() );
    REQUIRE( file.fileHeader().phoff == 50 );
    REQUIRE( file.fileHeader().shoff == 10000 );
    REQUIRE( file.programHeaderTable().headerCount() == 2 );
    REQUIRE( file.programHeaderTable().headerAt(0).segmentType() == SegmentType::ProgramHeaderTable );
    REQUIRE( file.programHeaderTable().headerAt(0).offset == 50 );
    REQUIRE( file.programHeaderTable().headerAt(0).vaddr == 50 );
    REQUIRE( file.programHeaderTable().headerAt(0).paddr == 50 );
    REQUIRE( file.programHeaderTable().headerAt(1).segmentType() == SegmentType::Dynamic );
    REQUIRE( file.programHeaderTable().headerAt(1).offset == 1000 );
    REQUIRE( file.programHeaderTable().headerAt(1).vaddr == 1100 );
    REQUIRE( file.programHeaderTable().headerAt(1).paddr == 1100 );
    REQUIRE( file.sectionHeaderTable().size() == 4 );
    REQUIRE( file.sectionHeaderTable()[1].sectionType() == SectionType::Dynamic );
    REQUIRE( file.sectionHeaderTable()[1].offset == 1000 );
    REQUIRE( file.sectionHeaderTable()[1].addr == 1100 );
    REQUIRE( file.sectionHeaderTable()[2].sectionType() == SectionType::StringTable );
    REQUIRE( file.sectionHeaderTable()[2].offset == 100 );
    REQUIRE( file.sectionHeaderTable()[2].addr == 110 );
    REQUIRE( file.dynamicSectionOffset() == 1000 );
    REQUIRE( file.dynamicStringTableOffset() == 100 );
    REQUIRE( file.dynamicSection().getStringTableAddress() == 110 );
    REQUIRE( file.dynamicSection().getRunPath() == QLatin1String("/tmp") );
    REQUIRE( file.headers().sectionNameStringTableHeader().offset == 5'000 );
  }
}

TEST_CASE("moveFirstCountSectionsToEnd")
{
  TestFileSetup setup;
  std::vector<uint16_t> movedSectionHeadersIndexes;

  setup.programHeaderTableOffset = 50;

  SECTION("gcc dynamic executable")
  {

  }
}

TEST_CASE("setRunPath")
{
  TestFileSetup setup;
  setup.programHeaderTableOffset = 50;
  setup.dynamicStringTableOffset = 100;
  setup.dynamicStringTableAddress = 100;
  setup.dynamicSectionOffset = 1000;
  setup.dynamicSectionAddress = 1000;
  setup.sectionNameStringTableOffset = 5'000;
  setup.sectionHeaderTableOffset = 10000;

  SECTION("There is initially no RUNPATH")
  {
    FileWriterFile file = makeWriterFile(setup);
    REQUIRE( file.dynamicSection().getRunPath().isEmpty() );

    file.setRunPath( QLatin1String("/tmp") );
    REQUIRE( file.dynamicSection().getRunPath() == QLatin1String("/tmp") );
  }

  SECTION("Change RUNPATH from /tmp /usr/lib")
  {
    setup.runPath = QLatin1String("/tmp");
    FileWriterFile file = makeWriterFile(setup);
    REQUIRE( file.dynamicSection().getRunPath() == QLatin1String("/tmp") );

    file.setRunPath( QLatin1String("/usr/lib") );
    REQUIRE( file.dynamicSection().getRunPath() == QLatin1String("/usr/lib") );
  }
}

/*
 * Some responsabilities are given to FileAllHeaders,
 * so we have to concentrate mainly on coordination
 * between headers, dynamic section and the logic here,
 * but not every details.
 */
TEST_CASE("setRunPath_fileLayout")
{
  using Mdt::DeployUtils::Impl::Elf::Class;

  TestFileSetup setup;
  FileWriterFileLayout originalLayout;
  FileWriterFile file;
  uint64_t stringTableSize;

  setup.programHeaderTableOffset = 50;
  setup.dynamicStringTableOffset = 100;
  setup.dynamicStringTableAddress = 200;
  setup.dynamicSectionOffset = 1'000;
  setup.dynamicSectionAddress = 1'200;
  setup.dynSymOffset = 500;
  setup.sectionNameStringTableOffset = 5'000;
  setup.sectionHeaderTableOffset = 10'000;

  SECTION("there is initially no RUNPATH")
  {
    file = makeWriterFile(setup);
    originalLayout = FileWriterFileLayout::fromFile( file.headers(), file.dynamicSection() );
    REQUIRE( !file.dynamicSection().containsRunPathEntry() );

    SECTION("set a RUNPATH - We not check dynamic string table here")
    {
      file.setRunPath( QLatin1String("/opt") );

      SECTION("the section header table does not change")
      {
        REQUIRE( file.fileHeader().shoff == setup.sectionHeaderTableOffset );
      }

      SECTION("the program header table will move to end (we add a PT_LOAD segment)")
      {
        REQUIRE( file.fileHeader().phoff >= originalLayout.globalOffsetRange().end() );
        REQUIRE( file.headers().programHeaderTableProgramHeader().offset == file.fileHeader().phoff );
      }

      SECTION("the dynamic section will move to the end, after PT_PHDR")
      {
        REQUIRE( file.dynamicProgramHeader().offset >= file.headers().programHeaderTableProgramHeader().fileOffsetEnd() );
        REQUIRE( file.dynamicSectionHeader().offset == file.dynamicProgramHeader().offset );
        REQUIRE( file.dynamicSectionHeader().size == file.dynamicProgramHeader().filesz );
        REQUIRE( file.dynamicSectionMovesToEnd() );
      }

      SECTION("the new size of the dynamic section must be reflected to the headers")
      {
        const uint64_t dynamicSectionSize = static_cast<uint64_t>( file.dynamicSection().byteCount(Class::Class64) );

        REQUIRE( file.dynamicProgramHeader().memsz == dynamicSectionSize );
        REQUIRE( file.dynamicProgramHeader().filesz == dynamicSectionSize );
        REQUIRE( file.dynamicSectionHeader().size == dynamicSectionSize );
      }

      SECTION("TODO: check global offset table")
      {
        ///REQUIRE( false );
      }
    }
  }

  SECTION("there is initially a RUNPATH")
  {
    setup.runPath = QLatin1String("/opt/libA");

    file = makeWriterFile(setup);
    REQUIRE( file.containsDynamicSection() );
    REQUIRE( file.containsDynamicStringTableSectionHeader() );
    originalLayout = FileWriterFileLayout::fromFile( file.headers(), file.dynamicSection() );

    SECTION("replace RUNPATH with a other one that is shorter")
    {
      file.setRunPath( QLatin1String("/opt") );
      stringTableSize = 1+4+1;

      SECTION("the dynamic section will not change")
      {
        REQUIRE( file.dynamicProgramHeader().offset == setup.dynamicSectionOffset );
        REQUIRE( file.dynamicProgramHeader().filesz == file.dynamicSection().byteCount(Class::Class64) );
        REQUIRE( file.dynamicSectionHeader().offset == setup.dynamicSectionOffset );
        REQUIRE( file.dynamicSectionHeader().size == file.dynamicSection().byteCount(Class::Class64) );
        REQUIRE( !file.dynamicSectionMovesToEnd() );
      }

      SECTION("the dynamic string table shrinks but stays at the same place")
      {
        REQUIRE( file.dynamicStringTableSectionHeader().offset == setup.dynamicStringTableOffset );
        REQUIRE( file.dynamicStringTableSectionHeader().size == stringTableSize );
        REQUIRE( file.originalDynamicStringTableOffsetRange().begin() == setup.dynamicStringTableOffset );
        REQUIRE( file.originalDynamicStringTableOffsetRange().byteCount() == setup.stringTableByteCount() );
        REQUIRE( file.dynamicStringTableOffsetRange().begin() == setup.dynamicStringTableOffset );
        REQUIRE( file.dynamicStringTableOffsetRange().byteCount() == stringTableSize );
        REQUIRE( !file.dynamicStringTableMovesToEnd() );
      }

      SECTION("the program header table does not change")
      {
        REQUIRE( file.fileHeader().phoff == setup.programHeaderTableOffset );
      }

      SECTION("the section header table does not change")
      {
        REQUIRE( file.fileHeader().shoff == setup.sectionHeaderTableOffset );
      }
    }

    SECTION("replace RUNPATH with a other one that is the same length")
    {
      file.setRunPath( QLatin1String("/opt/libB") );

      SECTION("the dynamic section will not change")
      {
        REQUIRE( file.dynamicProgramHeader().offset == setup.dynamicSectionOffset );
        REQUIRE( file.dynamicProgramHeader().filesz == file.dynamicSection().byteCount(Class::Class64) );
        REQUIRE( file.dynamicSectionHeader().offset == setup.dynamicSectionOffset );
        REQUIRE( file.dynamicSectionHeader().size == file.dynamicSection().byteCount(Class::Class64) );
        REQUIRE( !file.dynamicSectionMovesToEnd() );
      }

      SECTION("the dynamic string table will not change in term of layout")
      {
        REQUIRE( file.dynamicStringTableSectionHeader().offset == setup.dynamicStringTableOffset );
        REQUIRE( file.dynamicStringTableSectionHeader().size == setup.stringTableByteCount() );
        REQUIRE( file.originalDynamicStringTableOffsetRange().begin() == setup.dynamicStringTableOffset );
        REQUIRE( file.originalDynamicStringTableOffsetRange().byteCount() == setup.stringTableByteCount() );
        REQUIRE( file.dynamicStringTableOffsetRange().begin() == setup.dynamicStringTableOffset );
        REQUIRE( file.dynamicStringTableOffsetRange().byteCount() == setup.stringTableByteCount() );
        REQUIRE( !file.dynamicStringTableMovesToEnd() );
      }

      SECTION("the program header table does not change")
      {
        REQUIRE( file.fileHeader().phoff == setup.programHeaderTableOffset );
      }

      SECTION("the section header table does not change")
      {
        REQUIRE( file.fileHeader().shoff == setup.sectionHeaderTableOffset );
      }
    }

    SECTION("replace RUNPATH with a other one that is much longer")
    {
      const QString runPath = generateStringWithNChars(10000);
      file.setRunPath(runPath);
      stringTableSize = static_cast<uint64_t>(1+runPath.size()+1);

      SECTION("the section header table does not change")
      {
        REQUIRE( file.fileHeader().shoff == setup.sectionHeaderTableOffset );
      }

      SECTION("the program header table will move to end (we add a PT_LOAD segment)")
      {
        REQUIRE( file.fileHeader().phoff >= originalLayout.globalOffsetRange().end() );
        REQUIRE( file.headers().programHeaderTableProgramHeader().offset == file.fileHeader().phoff );
      }

      SECTION("the dynamic section will not change")
      {
        REQUIRE( file.dynamicProgramHeader().offset == setup.dynamicSectionOffset );
        REQUIRE( file.dynamicProgramHeader().filesz == file.dynamicSection().byteCount(Class::Class64) );
        REQUIRE( file.dynamicSectionHeader().offset == setup.dynamicSectionOffset );
        REQUIRE( file.dynamicSectionHeader().size == file.dynamicSection().byteCount(Class::Class64) );
        REQUIRE( !file.dynamicSectionMovesToEnd() );
      }

      SECTION("the dynamic string table grows and moves to the end, after PT_PHDR")
      {
        REQUIRE( file.dynamicStringTableSectionHeader().offset >= file.headers().programHeaderTableProgramHeader().fileOffsetEnd() );
        REQUIRE( file.dynamicStringTableSectionHeader().size == stringTableSize );
        REQUIRE( file.originalDynamicStringTableOffsetRange().begin() == setup.dynamicStringTableOffset );
        REQUIRE( file.originalDynamicStringTableOffsetRange().byteCount() == setup.stringTableByteCount() );
//         REQUIRE( file.dynamicStringTableOffsetRange().begin() == originalLayout.globalOffsetRange().end() );
//         REQUIRE( file.dynamicStringTableOffsetRange().byteCount() == stringTableSize );
        REQUIRE( file.dynamicStringTableMovesToEnd() );
      }

      SECTION("the dynamic section must have the new address of the dynamic string table")
      {
        REQUIRE( file.dynamicSection().getStringTableAddress() == file.headers().dynamicStringTableSectionHeader().addr );
        REQUIRE( file.dynamicSection().getStringTableSize() == stringTableSize );
      }
    }
  }
}

TEST_CASE("minimumSizeToWriteFile")
{
  TestFileSetup setup;
  FileWriterFile file;
  int64_t expectedMinimumSize;

  SECTION("section header table is at the end of the file (the common case)")
  {
    setup.programHeaderTableOffset = 50;
    setup.dynamicSectionOffset = 100;
    setup.dynamicSectionAddress = 100;
    setup.dynamicStringTableOffset = 1'000;
    setup.dynamicStringTableAddress = 1000;
    setup.sectionHeaderTableOffset = 10'000;
    file = makeWriterFile(setup);

    expectedMinimumSize = file.fileHeader().minimumSizeToReadAllSectionHeaders();
    REQUIRE( file.minimumSizeToWriteFile() == expectedMinimumSize );
  }

  SECTION("the dynamic section string table is at the end of the file")
  {
    setup.programHeaderTableOffset = 50;
    setup.dynamicSectionOffset = 100;
    setup.dynamicSectionAddress = 100;
    setup.dynamicStringTableOffset = 10'000;
    setup.dynamicStringTableAddress = 10'000;
    setup.sectionHeaderTableOffset = 2'000;
    file = makeWriterFile(setup);

    expectedMinimumSize = 10'000 + file.dynamicStringTableSize();
    REQUIRE( file.minimumSizeToWriteFile() == expectedMinimumSize );
  }
}

/*
 * Validity checks between headers counts and the file header
 * are the responsability of FileAllHeaders,
 * and also tested by its tests
 */
TEST_CASE("seemsValid")
{
  FileWriterFile file;
//   FileAllHeaders headers;
//   DynamicSection dynamicSection;
//   std::vector<ProgramHeader> programHeaderTable;
//   std::vector<SectionHeader> sectionHeaderTable;

//   FileHeader fileHeader = make64BitLittleEndianFileHeader();
//   fileHeader.phnum = 0;
//   fileHeader.shnum = 0;
//   REQUIRE( fileHeader.seemsValid() );

//   headers.setFileHeader(fileHeader);

  SECTION("default constructed")
  {
    REQUIRE( !file.seemsValid() );
  }
}
