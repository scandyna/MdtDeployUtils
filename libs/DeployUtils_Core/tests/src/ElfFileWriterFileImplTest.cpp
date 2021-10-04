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
#include "ElfFileIoTestUtils.h"
#include "ElfDynamicSectionImplTestCommon.h"
#include "ElfProgramHeaderTestUtils.h"
#include "Mdt/DeployUtils/Impl/Elf/FileWriterFile.h"
#include "Mdt/DeployUtils/Impl/Elf/FileOffsetChanges.h"
#include <QLatin1String>

using Mdt::DeployUtils::Impl::Elf::FileWriterFile;
using Mdt::DeployUtils::Impl::Elf::FileWriterFileLayout;
using Mdt::DeployUtils::Impl::Elf::FileAllHeaders;
using Mdt::DeployUtils::Impl::Elf::DynamicSection;
using Mdt::DeployUtils::Impl::Elf::FileHeader;
using Mdt::DeployUtils::Impl::Elf::ProgramHeader;
using Mdt::DeployUtils::Impl::Elf::ProgramHeaderTable;
using Mdt::DeployUtils::Impl::Elf::SectionHeader;

FileAllHeaders makeBasicFileAllHeaders()
{
  FileAllHeaders headers;
  FileHeader fileHeader = make64BitLittleEndianFileHeader();
  fileHeader.phnum = 0;
  fileHeader.shnum = 0;
  headers.setFileHeader(fileHeader);

  return headers;
}

SectionHeader makeNullSectionHeader()
{
  SectionHeader header;
  header.type = 0;

  return header;
}

SectionHeader makeDynamicSectionHeader()
{
  SectionHeader header;
  header.type = 6;

  return header;
}

SectionHeader makeStringTableSectionHeader()
{
  SectionHeader header;
  header.type = 3;

  return header;
}

struct TestFileSetup
{
  uint64_t programHeaderTableOffset;
  uint64_t sectionHeaderTableOffset;
  uint64_t dynamicSectionOffset;
  uint64_t dynamicSectionAddress;
  uint64_t dynamicStringTableOffset;
  uint64_t dynamicStringTableAddress;
  QString runPath;
  uint64_t sectionNameStringTableOffset;

  uint64_t stringTableByteCount() const
  {
    if( runPath.isEmpty() ){
      return 1;
    }
    return 1 + static_cast<uint64_t>( runPath.size() ) + 1;
  }
};

FileWriterFile makeWriterFile(const TestFileSetup & setup)
{
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

  SectionHeader dynamicSectionHeader = makeDynamicSectionHeader();
  dynamicSectionHeader.name = ".dynamic";
  dynamicSectionHeader.offset = setup.dynamicSectionOffset;
  dynamicSectionHeader.size = dynamicSectionSize;
  dynamicSectionHeader.addr = setup.dynamicSectionAddress;
  dynamicSectionHeader.link = 2;

  SectionHeader dynamicStringTableSectionHeader = makeStringTableSectionHeader();
  dynamicStringTableSectionHeader.name = ".dynstr";
  dynamicStringTableSectionHeader.offset = setup.dynamicStringTableOffset;
  dynamicStringTableSectionHeader.size = dynamicStringTableSize;
  dynamicStringTableSectionHeader.addr = setup.dynamicStringTableAddress;

  SectionHeader sectionNameStringTableHeader = makeStringTableSectionHeader();
  sectionNameStringTableHeader.name = "shstrtab";
  sectionNameStringTableHeader.offset = setup.sectionNameStringTableOffset;

  ProgramHeaderTable programHeaderTable;
  programHeaderTable.addHeaderFromFile(dynamicSectionProgramHeader);

  std::vector<SectionHeader> sectionHeaderTable;
  sectionHeaderTable.push_back( makeNullSectionHeader() );
  sectionHeaderTable.push_back(dynamicSectionHeader);
  sectionHeaderTable.push_back(dynamicStringTableSectionHeader);
  sectionHeaderTable.push_back(sectionNameStringTableHeader);

  fileHeader.shstrndx = 3;

  FileAllHeaders allHeaders;
  allHeaders.setFileHeader(fileHeader);
  allHeaders.setProgramHeaderTable(programHeaderTable);
  allHeaders.setSectionHeaderTable(sectionHeaderTable);

  return FileWriterFile::fromOriginalFile(allHeaders, dynamicSection);
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
    setup.dynamicStringTableOffset = 1'000;
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
    REQUIRE( file.programHeaderTable().headerCount() == 1 );
    REQUIRE( file.programHeaderTable().headerAt(0).segmentType() == SegmentType::Dynamic );
    REQUIRE( file.programHeaderTable().headerAt(0).offset == 1000 );
    REQUIRE( file.programHeaderTable().headerAt(0).vaddr == 1100 );
    REQUIRE( file.programHeaderTable().headerAt(0).paddr == 1100 );
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
    REQUIRE( file.programHeaderTable().headerCount() == 1 );
    REQUIRE( file.programHeaderTable().headerAt(0).segmentType() == SegmentType::Dynamic );
    REQUIRE( file.programHeaderTable().headerAt(0).offset == 1000 );
    REQUIRE( file.programHeaderTable().headerAt(0).vaddr == 1100 );
    REQUIRE( file.programHeaderTable().headerAt(0).paddr == 1100 );
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

TEST_CASE("setRunPath")
{
  TestFileSetup setup;
  setup.programHeaderTableOffset = 50;
  setup.dynamicStringTableOffset = 100;
  setup.dynamicSectionOffset = 1000;
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

TEST_CASE("setRunPath_fileLayout")
{
  using Mdt::DeployUtils::Impl::Elf::Class;

  TestFileSetup setup;
  FileWriterFileLayout originalLayout;
  FileWriterFile file;
  uint64_t stringTableSize;

  // This is the case for 64-bit architecture
  const uint64_t dynamicEntrySize = sizeof(DynamicStruct);

  SECTION("there is initially a RUNPATH")
  {
    setup.programHeaderTableOffset = 50;
    setup.dynamicStringTableOffset = 100;
    setup.dynamicSectionOffset = 1'000;
    setup.sectionNameStringTableOffset = 5'000;
    setup.sectionHeaderTableOffset = 10'000;
    setup.runPath = QLatin1String("/opt/libA");

    file = makeWriterFile(setup);
    REQUIRE( file.containsDynamicSection() );
    REQUIRE( file.containsDynamicStringTableSectionHeader() );

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
  }

//   SECTION(".dynamic before .dynstr - DEPRECATED")
//   {
//     setup.programHeaderTableOffset = 50;
//     setup.dynamicSectionOffset = 100;
//     setup.dynamicStringTableOffset = 1'000;
//     setup.sectionNameStringTableOffset = 5'000;
//     setup.sectionHeaderTableOffset = 10'000;
// 
//     SECTION("there is initially no RUNPATH")
//     {
//       file = makeWriterFile(setup);
//       REQUIRE( !file.dynamicSection().containsRunPathEntry() );
//       originalLayout = makeFileLayoutFromFile(file);
// 
//       file.setRunPath( QLatin1String("/tmp") );
//       REQUIRE( file.containsDynamicSection() );
//       REQUIRE( !file.dynamicSectionIsAfterDynamicStringTable() );
// //       REQUIRE( file.originalDynamicSectionEndOffset() == originalLayout.dynamicSectionEndOffset() );
// //       REQUIRE( file.originalDynamicStringTableEndOffset() == originalLayout.dynamicStringTableEndOffset() );
//       // We added the DT_RUNPATH entry + /tmp\0 string
//       REQUIRE( file.dynamicSectionChangesOffset() == dynamicEntrySize );
//       REQUIRE( file.dynamicStringTableChangesOffset() == 4+1 );
//       REQUIRE( file.globalChangesOffset() == dynamicEntrySize + 4+1 );
//       REQUIRE( file.dynamicProgramHeader().offset == 100 );
//       REQUIRE( file.dynamicProgramHeader().filesz == file.dynamicSection().byteCount(Class::Class64) );
//       REQUIRE( file.dynamicSectionHeader().offset == 100 );
//       REQUIRE( file.dynamicSectionHeader().size == file.dynamicSection().byteCount(Class::Class64) );
//       // .dynstr shifts the size of the new entry
//       REQUIRE( file.headers().dynamicStringTableSectionHeader().size == file.dynamicSection().stringTable().byteCount() );
//       REQUIRE( file.headers().dynamicStringTableSectionHeader().offset == 1000 + dynamicEntrySize );
//       REQUIRE( file.dynamicStringTableOffset() == file.headers().dynamicStringTableSectionHeader().offset );
//       REQUIRE( file.fileHeader().phoff == 50 );
//       // section name string table is after dynamic section and dynamic string table
//       REQUIRE( file.headers().sectionNameStringTableHeader().offset == 5'000 + dynamicEntrySize + 4+1 );
//       // section header table is at end, so it shifts the size of the new entry + new string
//       REQUIRE( file.fileHeader().shoff == 10'000 + dynamicEntrySize + 4+1 );
//     }
//   }

//   SECTION(".dynamic afetr .dynstr - DEPRECATED")
//   {
//     setup.programHeaderTableOffset = 50;
//     setup.dynamicStringTableOffset = 100;
//     setup.dynamicSectionOffset = 1'000;
//     setup.sectionNameStringTableOffset = 5'000;
//     setup.sectionHeaderTableOffset = 10'000;
// 
//     SECTION("there is initially no RUNPATH")
//     {
//       file = makeWriterFile(setup);
//       REQUIRE( !file.dynamicSection().containsRunPathEntry() );
//       originalLayout = makeFileLayoutFromFile(file);
// 
//       file.setRunPath( QLatin1String("/tmp") );
//       REQUIRE( file.containsDynamicSection() );
//       REQUIRE( file.dynamicSectionIsAfterDynamicStringTable() );
// //       REQUIRE( file.originalDynamicSectionEndOffset() == originalLayout.dynamicSectionEndOffset() );
// //       REQUIRE( file.originalDynamicStringTableEndOffset() == originalLayout.dynamicStringTableEndOffset() );
//       // We added the DT_RUNPATH entry + /tmp\0 string
//       REQUIRE( file.dynamicSectionChangesOffset() == dynamicEntrySize );
//       REQUIRE( file.dynamicStringTableChangesOffset() == 4+1 );
//       REQUIRE( file.globalChangesOffset() == dynamicEntrySize + 4+1 );
//       REQUIRE( file.dynamicProgramHeader().offset == 1'000 + 4+1 );
//       REQUIRE( file.dynamicProgramHeader().filesz == file.dynamicSection().byteCount(Class::Class64) );
//       REQUIRE( file.dynamicSectionHeader().offset == 1'000 + 4+1 );
//       REQUIRE( file.dynamicSectionHeader().size == file.dynamicSection().byteCount(Class::Class64) );
//       REQUIRE( file.headers().dynamicStringTableSectionHeader().size == file.dynamicSection().stringTable().byteCount() );
//       REQUIRE( file.headers().dynamicStringTableSectionHeader().offset == 100 );
//       REQUIRE( file.dynamicStringTableOffset() == file.headers().dynamicStringTableSectionHeader().offset );
//       REQUIRE( file.fileHeader().phoff == 50 );
//       // section name string table is after dynamic section and dynamic string table
//       REQUIRE( file.headers().sectionNameStringTableHeader().offset == 5'000 + dynamicEntrySize + 4+1 );
//       // section header table is at end, so it shifts the size of the new entry + new string
//       REQUIRE( file.fileHeader().shoff == 10'000 + dynamicEntrySize + 4+1 );
//     }
//   }
}

// TEST_CASE("globalOffsetRange")
// {
//   TestFileSetup setup;
//   FileWriterFile file;
//   int64_t expectedMinimumSize;
// 
//   SECTION("section header table is at the end of the file (the common case)")
//   {
//     setup.programHeaderTableOffset = 50;
//     setup.dynamicSectionOffset = 100;
//     setup.dynamicStringTableOffset = 1'000;
//     setup.sectionHeaderTableOffset = 10'000;
//     file = makeWriterFile(setup);
// 
//     expectedMinimumSize = file.fileHeader().minimumSizeToReadAllSectionHeaders();
// 
//     REQUIRE( file.globalOffsetRange().begin() == 0 );
//     REQUIRE( file.globalOffsetRange().minimumSizeToAccessRange() == expectedMinimumSize );
//   }
// 
//   SECTION("the dynamic section string table is at the end of the file")
//   {
//     setup.programHeaderTableOffset = 50;
//     setup.dynamicSectionOffset = 100;
//     setup.dynamicStringTableOffset = 10'000;
//     setup.sectionHeaderTableOffset = 2'000;
//     file = makeWriterFile(setup);
// 
//     expectedMinimumSize = setup.dynamicStringTableOffset + setup.stringTableByteCount();
// 
//     REQUIRE( file.globalOffsetRange().begin() == 0 );
//     REQUIRE( file.globalOffsetRange().minimumSizeToAccessRange() == expectedMinimumSize );
//   }
// }

TEST_CASE("minimumSizeToWriteFile")
{
  TestFileSetup setup;
  FileWriterFile file;
  int64_t expectedMinimumSize;

  SECTION("section header table is at the end of the file (the common case)")
  {
    setup.programHeaderTableOffset = 50;
    setup.dynamicSectionOffset = 100;
    setup.dynamicStringTableOffset = 1'000;
    setup.sectionHeaderTableOffset = 10'000;
    file = makeWriterFile(setup);

    expectedMinimumSize = file.fileHeader().minimumSizeToReadAllSectionHeaders();
    REQUIRE( file.minimumSizeToWriteFile() == expectedMinimumSize );
  }

  SECTION("the dynamic section string table is at the end of the file")
  {
    setup.programHeaderTableOffset = 50;
    setup.dynamicSectionOffset = 100;
    setup.dynamicStringTableOffset = 10'000;
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
  FileAllHeaders headers;
  DynamicSection dynamicSection;
  std::vector<ProgramHeader> programHeaderTable;
  std::vector<SectionHeader> sectionHeaderTable;

  FileHeader fileHeader = make64BitLittleEndianFileHeader();
  fileHeader.phnum = 0;
  fileHeader.shnum = 0;
  REQUIRE( fileHeader.seemsValid() );

  headers.setFileHeader(fileHeader);

  SECTION("default constructed")
  {
    REQUIRE( !file.seemsValid() );
  }

  SECTION("no dynamic segment and no dynamic headers")
  {
    file.setHeaders(headers);
    REQUIRE( file.seemsValid() );
  }

  SECTION("no dynamic segment but dynamic section header is present")
  {
  }

  SECTION("no dynamic segment but dynamic program header is present")
  {
  }

  /// also check index validity

  SECTION("dynamic section size")
  {
    dynamicSection.addEntry( makeNullEntry() );

    SECTION("header declares the correct bytes count")
    {
    }

    SECTION("header declares a wrong bytes count")
    {
    }
  }
}
