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
#include "Mdt/DeployUtils/Impl/Elf/FileWriterFile.h"
#include "Mdt/DeployUtils/Impl/Elf/FileOffsetChanges.h"
#include <QLatin1String>

using Mdt::DeployUtils::Impl::Elf::FileWriterFile;
using Mdt::DeployUtils::Impl::Elf::FileAllHeaders;
using Mdt::DeployUtils::Impl::Elf::DynamicSection;
using Mdt::DeployUtils::Impl::Elf::FileHeader;
using Mdt::DeployUtils::Impl::Elf::ProgramHeader;
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

ProgramHeader makeDynamicSectionProgramHeader()
{
  ProgramHeader header;
  header.type = 2;

  return header;
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
  uint64_t dynamicStringTableOffset;
  QString runPath;
};

FileWriterFile makeWriterFile(const TestFileSetup & setup)
{
  FileHeader fileHeader = make64BitLittleEndianFileHeader();
  fileHeader.phoff = setup.programHeaderTableOffset;
  fileHeader.shoff = setup.sectionHeaderTableOffset;

  DynamicSection dynamicSection;
  dynamicSection.addEntry( makeNullEntry() );
  dynamicSection.setRunPath(setup.runPath);

  const uint64_t dynamicSectionSize = dynamicSection.byteCount(fileHeader.ident._class);
  const uint64_t dynamicStringTableSize = dynamicSection.stringTable().byteCount();

  ProgramHeader dynamicSectionProgramHeader = makeDynamicSectionProgramHeader();
  dynamicSectionProgramHeader.offset = setup.dynamicSectionOffset;
  dynamicSectionProgramHeader.filesz = dynamicSectionSize;

  SectionHeader dynamicSectionHeader = makeDynamicSectionHeader();
  dynamicSectionHeader.offset = setup.dynamicSectionOffset;
  dynamicSectionHeader.size = dynamicSectionSize;
  dynamicSectionHeader.link = 2;

  SectionHeader dynamicStringTableSectionHeader = makeStringTableSectionHeader();
  dynamicStringTableSectionHeader.offset = setup.dynamicStringTableOffset;
  dynamicStringTableSectionHeader.size = dynamicStringTableSize;

  std::vector<ProgramHeader> programHeaderTable;
  programHeaderTable.push_back(dynamicSectionProgramHeader);

  std::vector<SectionHeader> sectionHeaderTable;
  sectionHeaderTable.push_back( makeNullSectionHeader() );
  sectionHeaderTable.push_back(dynamicSectionHeader);
  sectionHeaderTable.push_back(dynamicStringTableSectionHeader);

  FileAllHeaders allHeaders;
  allHeaders.setFileHeader(fileHeader);
  allHeaders.setProgramHeaderTable(programHeaderTable);
  allHeaders.setSectionHeaderTable(sectionHeaderTable);

  return FileWriterFile::fromOriginalFile(allHeaders, dynamicSection);
}


TEST_CASE("fromOriginalFile")
{
  using Impl::Elf::SegmentType;
  using Impl::Elf::SectionType;

  TestFileSetup setup;
  setup.programHeaderTableOffset = 50;
  setup.dynamicStringTableOffset = 100;
  setup.dynamicSectionOffset = 1000;
  setup.sectionHeaderTableOffset = 10000;

  FileWriterFile file = makeWriterFile(setup);
  REQUIRE( file.fileHeader().seemsValid() );
  REQUIRE( file.fileHeader().phoff == 50 );
  REQUIRE( file.fileHeader().shoff == 10000 );
  REQUIRE( file.programHeaderTable().size() == 1 );
  REQUIRE( file.programHeaderTable()[0].segmentType() == SegmentType::Dynamic );
  REQUIRE( file.programHeaderTable()[0].offset == 1000 );
  REQUIRE( file.sectionHeaderTable().size() == 3 );
  REQUIRE( file.sectionHeaderTable()[1].sectionType() == SectionType::Dynamic );
  REQUIRE( file.sectionHeaderTable()[1].offset == 1000 );
  REQUIRE( file.sectionHeaderTable()[2].sectionType() == SectionType::StringTable );
  REQUIRE( file.sectionHeaderTable()[2].offset == 100 );
}

TEST_CASE("setDynamicSectionAndStringTable")
{
  using Mdt::DeployUtils::Impl::Elf::FileHeader;
  using Mdt::DeployUtils::Impl::Elf::FileOffsetChanges;
  using Mdt::DeployUtils::Impl::Elf::Class;
  using Mdt::DeployUtils::Impl::Elf::DynamicStruct;

  FileWriterFile file;
  DynamicSection dynamicSection;
  std::vector<ProgramHeader> programHeaderTable;
  std::vector<SectionHeader> sectionHeaderTable;

  // This is the case for 64-bit architecture
  const uint64_t dynamicEntrySize = sizeof(DynamicStruct);

  int64_t fileSizeChange;

  FileHeader fileHeader = make64BitLittleEndianFileHeader();
  fileHeader.phoff = 50;
  fileHeader.shoff = 10000;

  ProgramHeader dynamicSectionProgramHeader;
  dynamicSectionProgramHeader.type = 2;
  dynamicSectionProgramHeader.offset = 100;
  dynamicSectionProgramHeader.filesz = 10;

//   programHeaderTable.push_back(dynamicSectionProgramHeader);

  SectionHeader dynamicSectionHeader = makeDynamicSectionHeader();
  dynamicSectionHeader.offset = 100;
  dynamicSectionHeader.size = 10;
  dynamicSectionHeader.link = 2;

  SectionHeader dynamicStringTableSectionHeader = makeStringTableSectionHeader();
  dynamicStringTableSectionHeader.offset = 1000;
  dynamicStringTableSectionHeader.size = 20;

  sectionHeaderTable.push_back( makeNullSectionHeader() );
//   sectionHeaderTable.push_back(dynamicSectionHeader);
//   sectionHeaderTable.push_back(dynamicStringTableSectionHeader);

  FileAllHeaders allHeaders;
  allHeaders.setFileHeader(fileHeader);
//   allHeaders.setProgramHeaderTable(programHeaderTable);
//   allHeaders.setSectionHeaderTable(sectionHeaderTable);
// 
//   file.setHeaders(allHeaders);

  REQUIRE( !file.containsDynamicSection() );

//   FileOffsetChanges offsetChanges;
//   offsetChanges.setOriginalSizes(dynamicSection, file.fileHeader().ident._class);

  dynamicSection.addEntry( makeStringTableSizeEntry(1) );
  
//   dynamicSection.setRunPath( QLatin1String("/tmp") );

//   file.setDynamicSectionAndStringTable(dynamicSection);
//   REQUIRE( file.containsDynamicSection() );
//   REQUIRE( file.dynamicProgramHeader().offset == 100 );
//   REQUIRE( file.dynamicProgramHeader().filesz == dynamicSection.byteCount(Class::Class64) );
//   REQUIRE( file.dynamicSectionHeader().offset == 100 );
//   REQUIRE( file.dynamicSectionHeader().size == dynamicSection.byteCount(Class::Class64) );
//   REQUIRE( file.headers().dynamicStringTableSectionHeader().offset == 1000 );
//   REQUIRE( file.headers().dynamicStringTableSectionHeader().size == dynamicSection.stringTable().byteCount() );

  SECTION(".dynstr after .dynamic")
  {
    dynamicSectionProgramHeader.offset = 100;
    dynamicSectionHeader.offset = 100;
    dynamicStringTableSectionHeader.offset = 1000;

    programHeaderTable.push_back(dynamicSectionProgramHeader);
    sectionHeaderTable.push_back(dynamicSectionHeader);
    sectionHeaderTable.push_back(dynamicStringTableSectionHeader);
    allHeaders.setProgramHeaderTable(programHeaderTable);
    allHeaders.setSectionHeaderTable(sectionHeaderTable);
    file.setHeaders(allHeaders);
    file.setDynamicSectionAndStringTable(dynamicSection);
    REQUIRE( file.containsDynamicSection() );

    SECTION("there is initially no RUNPATH")
    {
      dynamicSection.setRunPath( QLatin1String("/tmp") );

      fileSizeChange = file.setDynamicSectionAndStringTable(dynamicSection);
      REQUIRE( file.containsDynamicSection() );
      // We added the DT_RUNPATH entry + /tmp\0 string
      REQUIRE( fileSizeChange == dynamicEntrySize + 4+1 );
      REQUIRE( file.dynamicProgramHeader().offset == 100 );
      REQUIRE( file.dynamicProgramHeader().filesz == dynamicSection.byteCount(Class::Class64) );
      REQUIRE( file.dynamicSectionHeader().offset == 100 );
      REQUIRE( file.dynamicSectionHeader().size == dynamicSection.byteCount(Class::Class64) );
      // .dynstr shifts the size of the new entry
      REQUIRE( file.headers().dynamicStringTableSectionHeader().offset == 1000 + dynamicEntrySize );
      REQUIRE( file.headers().dynamicStringTableSectionHeader().size == dynamicSection.stringTable().byteCount() );
      REQUIRE( file.dynamicSection().getStringTableAddress() == file.headers().dynamicStringTableSectionHeader().offset );
      REQUIRE( file.fileHeader().phoff == 50 );
      // section header table is at end, so it shifts the size of the new entry + new string
      REQUIRE( file.fileHeader().shoff == 10000 + fileSizeChange );
    }
  }

  SECTION(".dynstr before .dynamic")
  {
    dynamicStringTableSectionHeader.offset = 100;
    dynamicSectionProgramHeader.offset = 1000;
    dynamicSectionHeader.offset = 1000;

    programHeaderTable.push_back(dynamicSectionProgramHeader);
    sectionHeaderTable.push_back(dynamicSectionHeader);
    sectionHeaderTable.push_back(dynamicStringTableSectionHeader);
    allHeaders.setProgramHeaderTable(programHeaderTable);
    allHeaders.setSectionHeaderTable(sectionHeaderTable);
    file.setHeaders(allHeaders);

    SECTION("there is initially no RUNPATH")
    {
      dynamicSection.setRunPath( QLatin1String("/tmp") );

      file.setDynamicSectionAndStringTable(dynamicSection);
      REQUIRE( file.containsDynamicSection() );
      // We added the DT_RUNPATH entry + /tmp\0 string
      REQUIRE( fileSizeChange == dynamicEntrySize + 4+1 );
      // .dynamic will shift the size of the new string
      REQUIRE( file.dynamicProgramHeader().offset == 1000 + 4+1 );
      REQUIRE( file.dynamicProgramHeader().filesz == dynamicSection.byteCount(Class::Class64) );
      REQUIRE( file.dynamicSectionHeader().offset == 1000 + 4+1 );
      REQUIRE( file.dynamicSectionHeader().size == dynamicSection.byteCount(Class::Class64) );
      REQUIRE( file.headers().dynamicStringTableSectionHeader().offset == 100 );
      REQUIRE( file.headers().dynamicStringTableSectionHeader().size == dynamicSection.stringTable().byteCount() );
      REQUIRE( file.dynamicSection().getStringTableAddress() == file.headers().dynamicStringTableSectionHeader().offset );
      REQUIRE( file.fileHeader().phoff == 50 );
      // section header table is at end, so it shifts the size of the new entry + new string
      REQUIRE( file.fileHeader().shoff == 10000 + fileSizeChange );
    }
  }

  /// \todo change arrangement to change offsets
  
  /// \todo DT_STRTAB entry of the dynamic section
  
  
  
  
}

TEST_CASE("minimumSizeToWriteFile")
{
  FileWriterFile file;
  FileAllHeaders headers;
  int64_t expectedMinimumSize;

  FileHeader fileHeader = make64BitLittleEndianFileHeader();

  ProgramHeader dynamicSectionProgramHeader = makeDynamicSectionProgramHeader();
  dynamicSectionProgramHeader.offset = 1000;

  std::vector<ProgramHeader> programHeaderTable;
  programHeaderTable.push_back(dynamicSectionProgramHeader);

  SectionHeader dynamicSectionHeader = makeDynamicSectionHeader();
  dynamicSectionHeader.offset = 1000;
  dynamicSectionHeader.link = 2;

  SectionHeader dynamicStringTableSectionHeader = makeStringTableSectionHeader();

  std::vector<SectionHeader> sectionHeaderTable;
  sectionHeaderTable.push_back( makeNullSectionHeader() );
  sectionHeaderTable.push_back(dynamicSectionHeader);

  DynamicSection dynamicSection;
  dynamicSection.addEntry( makeStringTableSizeEntry(1) );
  dynamicSection.setRunPath( QLatin1String("/tmp") );

  SECTION("section header table is at the end of the file (the common case)")
  {
    fileHeader.shoff = 10000;

    dynamicStringTableSectionHeader.offset = 2000;
    sectionHeaderTable.push_back(dynamicStringTableSectionHeader);

    headers.setFileHeader(fileHeader);
    headers.setProgramHeaderTable(programHeaderTable);
    headers.setSectionHeaderTable(sectionHeaderTable);
    file.setHeaders(headers);

    file.setDynamicSectionAndStringTable(dynamicSection);

    expectedMinimumSize = file.fileHeader().minimumSizeToReadAllSectionHeaders();
    REQUIRE( file.minimumSizeToWriteFile() == expectedMinimumSize );
  }

  SECTION("the dynamic section string table is at the end of the file (just for test)")
  {
    fileHeader.shoff = 2000;

    dynamicStringTableSectionHeader.offset = 10000;
    sectionHeaderTable.push_back(dynamicStringTableSectionHeader);

    headers.setFileHeader(fileHeader);
    headers.setProgramHeaderTable(programHeaderTable);
    headers.setSectionHeaderTable(sectionHeaderTable);
    file.setHeaders(headers);

    file.setDynamicSectionAndStringTable(dynamicSection);

    expectedMinimumSize = 10000 + dynamicSection.stringTable().byteCount();
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
