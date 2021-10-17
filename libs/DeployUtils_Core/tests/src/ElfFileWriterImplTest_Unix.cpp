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
#include "Mdt/DeployUtils/Impl/Elf/SymbolTableReader.h"
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

void openFile(QFile & file, const QString & executableFilePath, QIODevice::OpenMode mode)
{
  assert( !file.isOpen() );

  file.setFileName(executableFilePath);
  if( !file.open(mode) ){
    const QString msg = QLatin1String("open file '") + executableFilePath + QLatin1String(" failed: ") + file.errorString();
    throw QRuntimeError(msg);
  }
}

void openFileForWrite(QFile & file, const QString & executableFilePath)
{
  assert( !file.isOpen() );

  openFile(file, executableFilePath, QIODevice::ReadWrite);
}

void resizeFile(QFile & file, qint64 size)
{
  assert( file.isOpen() );
  assert( size > 0 );

  if( !file.resize(size) ){
    const QString msg = QLatin1String("resize file '") + file.fileName() + QLatin1String(" failed: ") + file.errorString();
    throw QRuntimeError(msg);
  }
}

ByteArraySpan mapFile(QFile & file)
{
  ByteArraySpan map;

  const qint64 size = file.size();
  map.data = file.map(0, size);
  map.size = size;

  if( map.isNull() ){
    const QString msg = QLatin1String("map file '") + file.fileName() + QLatin1String(" failed: ") + file.errorString();
    throw QRuntimeError(msg);
  }

  return map;
}

ByteArraySpan openAndMapFile(QFile & file, const QString & executableFilePath, QIODevice::OpenMode mode)
{
  assert( !file.isOpen() );

//   ByteArraySpan map;

  openFile(file, executableFilePath, mode);
//   file.setFileName(executableFilePath);
//   if( !file.open(mode) ){
//     const QString msg = QLatin1String("open file '") + executableFilePath + QLatin1String(" failed: ") + file.errorString();
//     throw QRuntimeError(msg);
//   }

  return mapFile(file);
//   const qint64 size = file.size();
//   map.data = file.map(0, size);
//   map.size = size;
// 
//   if( map.isNull() ){
//     const QString msg = QLatin1String("map file '") + executableFilePath + QLatin1String(" failed: ") + file.errorString();
//     throw QRuntimeError(msg);
//   }
// 
//   return map;
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

/** \todo Maybe also check back patched ELF with a tool, or readers
 *
 * Section headers may not be important when running the executable..
 */

FileWriterFile readElfFile(const QString & filePath)
{
  using Impl::Elf::extractFileHeader;
  using Impl::Elf::extractAllHeaders;
  using Impl::Elf::extractDynamicSection;
  using Impl::Elf::extractSectionsAssociationsSymTab;
  
  using Impl::Elf::SectionType;

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

  elfFile = FileWriterFile::fromOriginalFile( headers, extractDynamicSection( map, fileHeader, headers.sectionNameStringTableHeader() ) );

  /// \todo fix !
//   const auto symTabPred = [](const SectionHeader & header){
//     return header.sectionType() == SectionType::SymbolTable;
//   };
//   const auto dynSymHdrIt = std::find_if( elfFile.sectionHeaderTable().cbegin(), elfFile.sectionHeaderTable().cend(), symTabPred );
//   elfFile.setPartialSymbolTableFromFile( extractPartialSymbolTable(map, *dynSymHdrIt, headers.fileHeader().ident) );

  elfFile.setSectionSymbolTableFromFile( extractSectionsAssociationsSymTab( map, headers.fileHeader(), headers.sectionHeaderTable() ) );
//   elfFile.setSectionDynamicSymbolTableFromFile( extractSectionsAssociationsDynSym( map, headers.fileHeader(), headers.sectionHeaderTable() ) );

  if( !unmapAndCloseFile(file, map) ){
    const QString msg = QLatin1String("unmap/close file filed");
    throw QRuntimeError(msg);
  }

  return elfFile;
}

FileWriterFile copyAndReadElfFile(const QString & targetFilePath)
{
  if( !copyFile(QString::fromLocal8Bit(TEST_SIMPLE_EXECUTABLE_DYNAMIC_FILE_PATH), targetFilePath) ){
    const QString msg = QLatin1String("copy the file failed");
    throw QRuntimeError(msg);
  }

  if( !runExecutable(targetFilePath) ){
    const QString msg = QLatin1String("executing copied file failed");
    throw QRuntimeError(msg);
  }

  return readElfFile(targetFilePath);
}

bool readExecutable(const QString & filePath)
{
  using Impl::Elf::toDebugString;

  const FileWriterFile elfFile = readElfFile(filePath);
  if( !elfFile.seemsValid() ){
    return false;
  }

  std::cout << "File header:\n" << toDebugString(elfFile.fileHeader()).toStdString() << std::endl;
  std::cout << "Program headers:\n" << toDebugString(elfFile.programHeaderTable()).toStdString() << std::endl;
//   std::cout << "Section headers:\n" << toDebugString(elfFile.sectionHeaderTable()).toStdString() << std::endl;
//   std::cout << "Dynamic section:\n" << toDebugString( elfFile.dynamicSection() ).toStdString() << std::endl;
//   std::cout << "Dynamic section string table:\n" << toDebugString( elfFile.dynamicSection().stringTable() ).toStdString() << std::endl;

//   std::cout << "partial symbol table .symtab:\n" << toDebugString( elfFile.sectionSymbolTable() ).toStdString() << std::endl;
//   std::cout << "partial symbol table .dynsym:\n" << toDebugString( elfFile.sectionDynamicSymbolTable() ).toStdString() << std::endl;
//   std::cout << "Section header associated with the dynamic section:\n"
//             << toDebugString(elfFile.sectionHeaders[elfFile.dynamicSection.indexOfSectionHeader()]).toStdString() << std::endl;

//   std::cout << "Section header associated with the string table used by the dynamic section:\n"
//             << toDebugString(elfFile.sectionHeaders[elfFile.dynamicSection.stringTable().indexOfSectionHeader()]).toStdString() << std::endl;

  std::cout << "File layout:\n" << fileLayoutToDebugString( elfFile.fileHeader(), elfFile.programHeaderTable(), elfFile.sectionHeaderTable() ).toStdString() << std::endl;

  std::cout << sectionSegmentMappingToDebugString( elfFile.headers() ).toStdString() << std::endl;
  
  std::cout << "RunPath: " << elfFile.dynamicSection().getRunPath().toStdString() << std::endl;

  return true;
}

QString getExecutableRunPath(const QString & filePath)
{
  const FileWriterFile elfFile = readElfFile(filePath);
  if( !elfFile.seemsValid() ){
    return QString();
  }

  return elfFile.dynamicSection().getRunPath();
}

/*
 * Here we simply read a ELF executable
 * then write it back, without changing anything.
 *
 * The purpose is to check if writing the original headers works
 */
TEST_CASE("simpleReadWrite")
{
  using Impl::Elf::setFileToMap;

  QTemporaryDir dir;
  REQUIRE( dir.isValid() );
  dir.setAutoRemove(true);

  FileWriterFile elfFile;
  QFile file;
  ByteArraySpan map;

  const QString targetFilePath = makePath(dir, "targetFile");
  REQUIRE( copyFile(QString::fromLocal8Bit(TEST_SIMPLE_EXECUTABLE_DYNAMIC_FILE_PATH), targetFilePath) );
  REQUIRE( runExecutable(targetFilePath) );

  /// \todo just for debug - remove
  readExecutable(targetFilePath);
  
  elfFile = readElfFile(targetFilePath);
  REQUIRE( elfFile.seemsValid() );

//   std::cout << "File header:\n" << toDebugString(elfFile.fileHeader).toStdString() << std::endl;

//   std::cout << "Programe headers count: " << elfFile.programHeaders.size() << std::endl;
//   std::cout << "Section headers count: " << elfFile.sectionHeaders.size() << std::endl;
//   fileHeader.phnum = 3;
//   fileHeader.shstrndx = 30;

  map = openAndMapFileForWrite(file, targetFilePath);
  REQUIRE( !map.isNull() );

  setFileToMap(map, elfFile);

  unmapAndCloseFile(file, map);
  REQUIRE( runExecutable(targetFilePath) );

  /// \todo create a function that takes expected headers and reads the file back
  /*
   * Read the file again
   * (Sections are not used for execution)
   */
  REQUIRE( readExecutable(targetFilePath) );
}

/// \todo try: eu-elflint --gnu-ld --strict target

/// \todo see LD_BIN_NOW env var for tests

/** \todo see: alignment and paging / page size:
 *
 * A) https://fr.wikipedia.org/wiki/M%C3%A9moire_virtuelle
 * B) https://fr.wikipedia.org/wiki/Alignement_en_m%C3%A9moire
 *
 */

/// \todo see: https://stackoverflow.com/questions/46117065/required-alignment-of-text-versus-data


TEST_CASE("editRunPath")
{
  using Impl::Elf::setFileToMap;

  QTemporaryDir dir;
  REQUIRE( dir.isValid() );
  dir.setAutoRemove(false);

  FileWriterFile elfFile;
  QFile file;
  ByteArraySpan map;

  SECTION("dynamic section does not change")
  {
    const QString targetFilePath = makePath(dir, "no_changes");
    elfFile = copyAndReadElfFile(targetFilePath);
    REQUIRE( elfFile.seemsValid() );

    map = openAndMapFileForWrite(file, targetFilePath);
    REQUIRE( !map.isNull() );

    setFileToMap(map, elfFile);

    unmapAndCloseFile(file, map);
    REQUIRE( runExecutable(targetFilePath) );
    REQUIRE( readExecutable(targetFilePath) );
  }

  SECTION("change RUNPATH")
  {
    const QString targetFilePath = makePath(dir, "change_runpath");
    elfFile = copyAndReadElfFile(targetFilePath);
    REQUIRE( elfFile.seemsValid() );

    elfFile.setRunPath( QLatin1String("/tmp") );

    /// \todo sandbox
    ///map = openAndMapFileForWrite(file, targetFilePath);
    ///REQUIRE( !map.isNull() );
    openFileForWrite(file, targetFilePath);
    resizeFile( file, elfFile.minimumSizeToWriteFile() );
    map = mapFile(file);
    REQUIRE( !map.isNull() );

    setFileToMap(map, elfFile);

    unmapAndCloseFile(file, map);
    REQUIRE( runExecutable(targetFilePath) );
    REQUIRE( readExecutable(targetFilePath) );
    REQUIRE( getExecutableRunPath(targetFilePath) == QLatin1String("/tmp") );
  }

  SECTION("remove RUNPATH")
  {
    const QString targetFilePath = makePath(dir, "remove_runpath");
    elfFile = copyAndReadElfFile(targetFilePath);
    REQUIRE( elfFile.seemsValid() );

    elfFile.setRunPath( QString() );
//     REQUIRE( elfFile.globalChangesOffset() < 0 );

    /// \todo sandbox
    ///map = openAndMapFileForWrite(file, targetFilePath);
    ///REQUIRE( !map.isNull() );
    openFileForWrite(file, targetFilePath);
    resizeFile( file, elfFile.minimumSizeToWriteFile() );
    map = mapFile(file);
    REQUIRE( !map.isNull() );

    setFileToMap(map, elfFile);

    unmapAndCloseFile(file, map);
    REQUIRE( runExecutable(targetFilePath) );
    REQUIRE( readExecutable(targetFilePath) );
    REQUIRE( getExecutableRunPath(targetFilePath).isEmpty() );
  }

  SECTION("set a very long RUNPATH")
  {
    const QString targetFilePath = makePath(dir, "set_very_long_runpath");
    elfFile = copyAndReadElfFile(targetFilePath);
    REQUIRE( elfFile.seemsValid() );

    qDebug() << "set very long RUNPATH, file: " << targetFilePath;
    
    const QString runPath = generateStringWithNChars(10000);
    ///const QString runPath = elfFile.dynamicSection().getRunPath() + QLatin1String(":") + generateStringWithNChars(10000);
    
//     qDebug() << "RUNPATH: " << runPath;
//     DynamicSection dynamicSection = elfFile.dynamicSection();
//     dynamicSection.setRunPath(runPath);
//     fileSizeChange = elfFile.setDynamicSectionAndStringTable(dynamicSection);
//     REQUIRE( fileSizeChange > 0 );

    /// \todo just for debug
    qDebug() << "************* file " << targetFilePath << " before changes *************";
    readExecutable(targetFilePath);
    qDebug() << "*************************************************************************" ;
    
    elfFile.setRunPath(runPath);
    ///REQUIRE( elfFile.globalChangesOffset() > 0 );

    openFileForWrite(file, targetFilePath);
    resizeFile( file, elfFile.minimumSizeToWriteFile() );
    map = mapFile(file);
    REQUIRE( !map.isNull() );
    
//     REQUIRE( file.resize( elfFile.minimumSizeToWriteFile() ) );
//     map = openAndMapFileForWrite(file, targetFilePath);
//     REQUIRE( !map.isNull() );

    setFileToMap(map, elfFile);
//     setDynamicSectionToMap( map, elfFile.dynamicSectionHeader(), elfFile.dynamicSection(), elfFile.fileHeader() );
//     setStringTableToMap( map, elfFile.headers().dynamicStringTableSectionHeader(), elfFile.dynamicSection().stringTable() );
//     setAllHeadersToMap( map, elfFile.headers() );

    unmapAndCloseFile(file, map);
    
    qDebug() << "file " << targetFilePath << " written";
    
    REQUIRE( readExecutable(targetFilePath) );
    REQUIRE( runExecutable(targetFilePath) );
    REQUIRE( getExecutableRunPath(targetFilePath) == runPath );
  }
}

TEST_CASE("sandboxWithPatchelf")
{
  QTemporaryDir dir;
  REQUIRE( dir.isValid() );
  dir.setAutoRemove(false);

  const QString targetFilePath = makePath(dir, "targetFile");
  REQUIRE( copyFile(QString::fromLocal8Bit(TEST_SIMPLE_EXECUTABLE_DYNAMIC_FILE_PATH), targetFilePath) );
  REQUIRE( runExecutable(targetFilePath) );

  qDebug() << "************* file " << targetFilePath << " before changes *************";
  readExecutable(targetFilePath);
  qDebug() << "*************************************************************************" ;

  const QString patchelf = QLatin1String("patchelf");

  SECTION("remove RPATH")
  {
    qDebug() << "-----------------  remove RPATH";

    const QStringList arguments = QStringList({QLatin1String("--remove-rpath"),QLatin1String("--debug"),targetFilePath});

    REQUIRE( runExecutable(patchelf, arguments) );

    REQUIRE( readExecutable(targetFilePath) );
    REQUIRE( runExecutable(targetFilePath) );
    REQUIRE( getExecutableRunPath(targetFilePath).isEmpty() );
  }

  SECTION("set very long RPATH")
  {
    qDebug() << "----------------- set very long RPATH";

    const QString runPath = generateStringWithNChars(10000);
    const QStringList arguments = QStringList({QLatin1String("--set-rpath"),runPath,QLatin1String("--debug"),targetFilePath});

    REQUIRE( runExecutable(patchelf, arguments) );

    REQUIRE( readExecutable(targetFilePath) );
    REQUIRE( runExecutable(targetFilePath) );
    REQUIRE( getExecutableRunPath(targetFilePath) == runPath );
  }

  SECTION("remove RPATH then set a new RPATH")
  {
    qDebug() << "-----------------  remove RPATH then set a new RPATH +++++++++++++++++";

    QStringList arguments = QStringList({QLatin1String("--remove-rpath"),targetFilePath});

    REQUIRE( runExecutable(patchelf, arguments) );

    const QString runPath = generateStringWithNChars(10000);
    arguments = QStringList({QLatin1String("--set-rpath"),runPath,QLatin1String("--debug"),targetFilePath});

    REQUIRE( runExecutable(patchelf, arguments) );

    REQUIRE( readExecutable(targetFilePath) );
    REQUIRE( runExecutable(targetFilePath) );
    REQUIRE( getExecutableRunPath(targetFilePath) == runPath );
  }
}
