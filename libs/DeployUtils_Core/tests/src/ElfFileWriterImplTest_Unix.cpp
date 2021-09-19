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
#include "TestUtils.h"
#include "TestFileUtils.h"
#include "Mdt/DeployUtils/Impl/Elf/FileReader.h"
#include "Mdt/DeployUtils/Impl/Elf/ProgramHeaderReader.h"
#include "Mdt/DeployUtils/Impl/Elf/FileHeaderWriter.h"
#include "Mdt/DeployUtils/Impl/Elf/ProgramHeaderWriter.h"
#include "Mdt/DeployUtils/Impl/Elf/SectionHeaderWriter.h"
#include "Mdt/DeployUtils/Impl/Elf/DynamicSectionWriter.h"
#include "Mdt/DeployUtils/Impl/Elf/StringTableWriter.h"
#include "Mdt/DeployUtils/Impl/Elf/FileWriterFile.h"
#include "Mdt/DeployUtils/Impl/Elf/FileWriter.h"

#include "Mdt/DeployUtils/Impl/Elf/FileAllHeadersReader.h"

#include "Mdt/DeployUtils/QRuntimeError.h"
#include <QTemporaryDir>
#include <QFile>
#include <QString>
#include <QLatin1String>
#include <vector>
#include <cassert>

#include "Mdt/DeployUtils/Impl/Elf/Debug.h"
#include <iostream>

using namespace Mdt::DeployUtils;
using Impl::ByteArraySpan;
using Impl::Elf::FileHeader;
using Impl::Elf::ProgramHeader;
using Impl::Elf::SectionHeader;
using Impl::Elf::FileAllHeaders;
using Impl::Elf::DynamicSection;
using Impl::Elf::FileWriterFile;

ByteArraySpan openAndMapFile(QFile & file, const QString & executableFilePath, QIODevice::OpenMode mode)
{
  assert( !file.isOpen() );

  ByteArraySpan map;

  file.setFileName(executableFilePath);
  if( !file.open(mode) ){
    const QString msg = QLatin1String("open file '") + executableFilePath + QLatin1String(" failed: ") + file.errorString();
    throw QRuntimeError(msg);
  }

  const qint64 size = file.size();
  map.data = file.map(0, size);
  map.size = size;

  if( map.isNull() ){
    const QString msg = QLatin1String("map file '") + executableFilePath + QLatin1String(" failed: ") + file.errorString();
    throw QRuntimeError(msg);
  }

  return map;
}

ByteArraySpan openAndMapFileForRead(QFile & file, const QString & executableFilePath)
{
  assert( !file.isOpen() );

  return openAndMapFile(file, executableFilePath, QIODevice::ReadOnly);
}

ByteArraySpan openAndMapFileForWrite(QFile & file, const QString & executableFilePath)
{
  assert( !file.isOpen() );

  return openAndMapFile(file, executableFilePath, QIODevice::ReadWrite);
}

bool unmapAndCloseFile(QFile & file, ByteArraySpan & map)
{
  assert( file.isOpen() );
  assert( !map.isNull() );

  const QString filePath = file.fileName();

  if( !file.unmap(map.data) ){
    const QString msg = QLatin1String("unmap file '") + filePath + QLatin1String(" failed: ") + file.errorString();
    return false;
  }
  map.data = nullptr;
  map.size = 0;
  if( !file.flush() ){
    const QString msg = QLatin1String("flush file '") + filePath + QLatin1String(" failed: ") + file.errorString();
    return false;
  }
  file.close();

  return true;
}

struct TestElfFile
{
  FileHeader fileHeader;
  std::vector<ProgramHeader> programHeaders;
  std::vector<SectionHeader> sectionHeaders;
  DynamicSection dynamicSection;

  const SectionHeader & dynamicSectionHeader() const noexcept
  {
//     assert( dynamicSection.indexOfSectionHeader() < sectionHeaders.size() );
//     return sectionHeaders[dynamicSection.indexOfSectionHeader()];
  }

  void setDynamicSectionHeader(const SectionHeader & header) noexcept
  {
//     assert( dynamicSection.indexOfSectionHeader() < sectionHeaders.size() );
//     sectionHeaders[dynamicSection.indexOfSectionHeader()] = header;
  }

  const SectionHeader & dynamicStringTableSectionHeader() const noexcept
  {
//     assert( dynamicSection.stringTable().indexOfSectionHeader() < sectionHeaders.size() );

//     return sectionHeaders[dynamicSection.stringTable().indexOfSectionHeader()];
  }

  void setDynamicStringTableSectionHeader(const SectionHeader & header) noexcept
  {
//     assert( dynamicSection.stringTable().indexOfSectionHeader() < sectionHeaders.size() );

//     sectionHeaders[dynamicSection.stringTable().indexOfSectionHeader()] = header;
  }
};

/** \todo Maybe also check back patched ELF with a tool, or readers
 *
 * Section headers may not be important when running the executable..
 */

/*
TestElfFile readElfFile_Old(const QString & filePath)
{
  using Impl::Elf::extractFileHeader;
  using Impl::Elf::extractAllProgramHeaders;
  using Impl::Elf::extractAllSectionHeaders;
  using Impl::Elf::extractDynamicSection;

  TestElfFile elfFile;
  QFile file(filePath);
  ByteArraySpan map;

  map = openAndMapFileForRead(file, filePath);

  elfFile.fileHeader = extractFileHeader(map);
  if( !elfFile.fileHeader.seemsValid() ){
    const QString msg = QLatin1String("file does not contain a valid file header");
    throw QRuntimeError(msg);
  }
  elfFile.programHeaders = extractAllProgramHeaders(map, elfFile.fileHeader);
  elfFile.sectionHeaders = extractAllSectionHeaders(map, elfFile.fileHeader);
  elfFile.dynamicSection = extractDynamicSection(map, elfFile.fileHeader, elfFile.sectionHeaders[elfFile.fileHeader.shstrndx]);

  if( !unmapAndCloseFile(file, map) ){
    const QString msg = QLatin1String("unmap/close file filed");
    throw QRuntimeError(msg);
  }

  return elfFile;
}
*/

FileWriterFile readElfFile(const QString & filePath)
{
  using Impl::Elf::extractFileHeader;
  using Impl::Elf::extractAllHeaders;
  using Impl::Elf::extractDynamicSection;

  FileWriterFile elfFile;
  QFile file(filePath);
  ByteArraySpan map;

  map = openAndMapFileForRead(file, filePath);

  const FileHeader fileHeader = extractFileHeader(map);
  if( !fileHeader.seemsValid() ){
    const QString msg = QLatin1String("file does not contain a valid file header");
    throw QRuntimeError(msg);
  }

  const FileAllHeaders headers = extractAllHeaders(map, fileHeader);
  if( !headers.seemsValid() ){
    const QString msg = QLatin1String("file contains some invalid header");
    throw QRuntimeError(msg);
  }
  elfFile.setHeaders(headers);

  elfFile.setDynamicSectionAndStringTable( extractDynamicSection( map, fileHeader, headers.sectionNameStringTableHeader() ) );

  if( !unmapAndCloseFile(file, map) ){
    const QString msg = QLatin1String("unmap/close file filed");
    throw QRuntimeError(msg);
  }

  return elfFile;
}

bool readExecutable(const QString & filePath)
{
  using Impl::Elf::toDebugString;

  const FileWriterFile elfFile = readElfFile(filePath);
  if( !elfFile.seemsValid() ){
    return false;
  }

//   std::cout << "File header:\n" << toDebugString(elfFile.fileHeader).toStdString() << std::endl;
//   std::cout << "Program headers:\n" << toDebugString(elfFile.programHeaders).toStdString() << std::endl;
//   std::cout << "Section headers:\n" << toDebugString(elfFile.sectionHeaders).toStdString() << std::endl;
  std::cout << "Dynamic section:\n" << toDebugString( elfFile.dynamicSection() ).toStdString() << std::endl;
  std::cout << "Dynamic section string table:\n" << toDebugString( elfFile.dynamicSection().stringTable() ).toStdString() << std::endl;

//   std::cout << "Section header associated with the dynamic section:\n"
//             << toDebugString(elfFile.sectionHeaders[elfFile.dynamicSection.indexOfSectionHeader()]).toStdString() << std::endl;

//   std::cout << "Section header associated with the string table used by the dynamic section:\n"
//             << toDebugString(elfFile.sectionHeaders[elfFile.dynamicSection.stringTable().indexOfSectionHeader()]).toStdString() << std::endl;

  std::cout << "File layout:\n" << fileLayoutToDebugString( elfFile.fileHeader(), elfFile.programHeaderTable(), elfFile.sectionHeaderTable() ).toStdString() << std::endl;

  return true;
}

/*
 * Here we simply read a ELF executable
 * then write it back, without changing anything.
 *
 * The purpose is to check if writing the original headers works
 */
TEST_CASE("simpleReadWrite")
{
  using Impl::Elf::setAllHeadersToMap;

  QTemporaryDir dir;
  REQUIRE( dir.isValid() );
  dir.setAutoRemove(true);

  FileWriterFile elfFile;
  QFile file;
  ByteArraySpan map;

  const QString targetFilePath = makePath(dir, "targetFile");
  REQUIRE( copyFile(QString::fromLocal8Bit(TEST_SIMPLE_EXECUTABLE_DYNAMIC_FILE_PATH), targetFilePath) );
  REQUIRE( runExecutable(targetFilePath) );

  elfFile = readElfFile(targetFilePath);
  REQUIRE( elfFile.seemsValid() );

//   std::cout << "File header:\n" << toDebugString(elfFile.fileHeader).toStdString() << std::endl;

//   std::cout << "Programe headers count: " << elfFile.programHeaders.size() << std::endl;
//   std::cout << "Section headers count: " << elfFile.sectionHeaders.size() << std::endl;
//   fileHeader.phnum = 3;
//   fileHeader.shstrndx = 30;

  map = openAndMapFileForWrite(file, targetFilePath);
  REQUIRE( !map.isNull() );

  setAllHeadersToMap( map, elfFile.headers() );
//   setHeadersToMap(map, elfFile.fileHeader, elfFile.programHeaders, elfFile.sectionHeaders);

  unmapAndCloseFile(file, map);
  REQUIRE( runExecutable(targetFilePath) );

  /// \todo create a function that takes expected headers and reads the file back
  /*
   * Read the file again
   * (Sections are not used for execution)
   */
  REQUIRE( readExecutable(targetFilePath) );
}

TEST_CASE("DynamicSection")
{
  using Impl::Elf::setDynamicSectionToMap;
  using Impl::Elf::setStringTableToMap;
  using Impl::Elf::adjustedStringTableSectionHeader;
//   using Impl::Elf::adjustDynamicSectionHeader;
  using Impl::Elf::setAllHeadersToMap;

  QTemporaryDir dir;
  REQUIRE( dir.isValid() );
  dir.setAutoRemove(true);

  FileWriterFile elfFile;
  QFile file;
  ByteArraySpan map;

  const QString targetFilePath = makePath(dir, "targetFile");
  REQUIRE( copyFile(QString::fromLocal8Bit(TEST_SIMPLE_EXECUTABLE_DYNAMIC_FILE_PATH), targetFilePath) );
  REQUIRE( runExecutable(targetFilePath) );

  elfFile = readElfFile(targetFilePath);
  REQUIRE( elfFile.seemsValid() );

  SECTION("dynamic section does not change")
  {
    map = openAndMapFileForWrite(file, targetFilePath);
    REQUIRE( !map.isNull() );

    setDynamicSectionToMap( map, elfFile.dynamicSectionHeader(), elfFile.dynamicSection(), elfFile.fileHeader() );
    setStringTableToMap( map, elfFile.headers().dynamicStringTableSectionHeader(), elfFile.dynamicSection().stringTable() );
    setAllHeadersToMap( map, elfFile.headers() );
//     setHeadersToMap(map, elfFile.fileHeader, elfFile.programHeaders, elfFile.sectionHeaders);

    unmapAndCloseFile(file, map);
    REQUIRE( runExecutable(targetFilePath) );
    REQUIRE( readExecutable(targetFilePath) );
  }

  SECTION("change RUNPATH")
  {
    DynamicSection dynamicSection = elfFile.dynamicSection();
    dynamicSection.setRunPath( QLatin1String("/tmp") );
    elfFile.setDynamicSectionAndStringTable(dynamicSection);
//     elfFile.dynamicSection.setRunPath( QLatin1String("/tmp") );

//     std::cout << "********** string table: " << toDebugString(elfFile.dynamicSection.stringTable()).toStdString() << " *******" << std::endl;

//     SectionHeader dynamicSectionHeader = elfFile.dynamicSectionHeader();
//     adjustDynamicSectionHeader( dynamicSectionHeader, elfFile.dynamicSection(), elfFile.fileHeader() );
    //elfFile.setDynamicSectionHeader(dynamicSectionHeader);
    //elfFile.setDynamicStringTableSectionHeader( adjustedStringTableSectionHeader( elfFile.dynamicStringTableSectionHeader(), elfFile.dynamicSection.stringTable() ) );

    map = openAndMapFileForWrite(file, targetFilePath);
    REQUIRE( !map.isNull() );

    setDynamicSectionToMap( map, elfFile.dynamicSectionHeader(), elfFile.dynamicSection(), elfFile.fileHeader() );
    setStringTableToMap( map, elfFile.headers().dynamicStringTableSectionHeader(), elfFile.dynamicSection().stringTable() );
    setAllHeadersToMap( map, elfFile.headers() );
//     setDynamicSectionToMap(map, elfFile.dynamicSectionHeader(), elfFile.dynamicSection, elfFile.fileHeader);
//     setStringTableToMap( map, elfFile.dynamicStringTableSectionHeader(), elfFile.dynamicSection.stringTable() );
//     setHeadersToMap(map, elfFile.fileHeader, elfFile.programHeaders, elfFile.sectionHeaders);

    unmapAndCloseFile(file, map);
    REQUIRE( runExecutable(targetFilePath) );
    REQUIRE( readExecutable(targetFilePath) );
  }

  SECTION("remove RUNPATH")
  {
    DynamicSection dynamicSection = elfFile.dynamicSection();
    dynamicSection.removeRunPath();
    elfFile.setDynamicSectionAndStringTable(dynamicSection);
//     elfFile.dynamicSection.removeRunPath();

//     SectionHeader dynamicSectionHeader = elfFile.dynamicSectionHeader();
//     adjustDynamicSectionHeader(dynamicSectionHeader, elfFile.dynamicSection(), elfFile.fileHeader());
    //elfFile.setDynamicSectionHeader(dynamicSectionHeader);
    //elfFile.setDynamicStringTableSectionHeader( adjustedStringTableSectionHeader( elfFile.dynamicStringTableSectionHeader(), elfFile.dynamicSection.stringTable() ) );

    map = openAndMapFileForWrite(file, targetFilePath);
    REQUIRE( !map.isNull() );

    setDynamicSectionToMap( map, elfFile.dynamicSectionHeader(), elfFile.dynamicSection(), elfFile.fileHeader() );
    setStringTableToMap( map, elfFile.headers().dynamicStringTableSectionHeader(), elfFile.dynamicSection().stringTable() );
    setAllHeadersToMap( map, elfFile.headers() );
//     setDynamicSectionToMap(map, elfFile.dynamicSectionHeader(), elfFile.dynamicSection, elfFile.fileHeader);
//     setStringTableToMap( map, elfFile.dynamicStringTableSectionHeader(), elfFile.dynamicSection.stringTable() );
//     setHeadersToMap(map, elfFile.fileHeader, elfFile.programHeaders, elfFile.sectionHeaders);

    unmapAndCloseFile(file, map);
    REQUIRE( runExecutable(targetFilePath) );
    REQUIRE( readExecutable(targetFilePath) );
  }

  SECTION("set a very long RUNPATH")
  {
    DynamicSection dynamicSection = elfFile.dynamicSection();
    dynamicSection.setRunPath( generateStringWithNChars(10000) );
    elfFile.setDynamicSectionAndStringTable(dynamicSection);
//     elfFile.dynamicSection.setRunPath( generateStringWithNChars(10000) );

//     std::cout << "********** string table: " << toDebugString(elfFile.dynamicSection.stringTable()).toStdString() << " *******" << std::endl;

//     SectionHeader dynamicSectionHeader = elfFile.dynamicSectionHeader();
//     adjustDynamicSectionHeader(dynamicSectionHeader, elfFile.dynamicSection(), elfFile.fileHeader());
    //elfFile.setDynamicSectionHeader(dynamicSectionHeader);
    //elfFile.setDynamicStringTableSectionHeader( adjustedStringTableSectionHeader( elfFile.dynamicStringTableSectionHeader(), elfFile.dynamicSection.stringTable() ) );

    map = openAndMapFileForWrite(file, targetFilePath);
    REQUIRE( !map.isNull() );

    setDynamicSectionToMap( map, elfFile.dynamicSectionHeader(), elfFile.dynamicSection(), elfFile.fileHeader() );
    setStringTableToMap( map, elfFile.headers().dynamicStringTableSectionHeader(), elfFile.dynamicSection().stringTable() );
    setAllHeadersToMap( map, elfFile.headers() );
//     setDynamicSectionToMap(map, elfFile.dynamicSectionHeader(), elfFile.dynamicSection, elfFile.fileHeader);
//     setStringTableToMap( map, elfFile.dynamicStringTableSectionHeader(), elfFile.dynamicSection.stringTable() );
//     setHeadersToMap(map, elfFile.fileHeader, elfFile.programHeaders, elfFile.sectionHeaders);

    unmapAndCloseFile(file, map);
    REQUIRE( readExecutable(targetFilePath) );
    REQUIRE( runExecutable(targetFilePath) );
  }
}
