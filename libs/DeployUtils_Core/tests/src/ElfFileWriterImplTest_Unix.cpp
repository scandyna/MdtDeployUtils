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
#include "Mdt/DeployUtils/QRuntimeError.h"
#include <QTemporaryDir>
#include <QFile>
#include <QString>
#include <QLatin1String>
#include <vector>
#include <cassert>

using namespace Mdt::DeployUtils;
using Impl::ByteArraySpan;
using Impl::Elf::FileHeader;
using Impl::Elf::ProgramHeader;
using Impl::Elf::SectionHeader;

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

/*
 * Here we simply read a ELF executable
 * then write it back, without changing anything.
 *
 * The purpose is to check if writing the original headers works
 */
TEST_CASE("simpleReadWrite")
{
  using Impl::Elf::extractFileHeader;
  using Impl::Elf::extractAllProgramHeaders;
  using Impl::Elf::extractAllSectionHeaders;

  QTemporaryDir dir;
  REQUIRE( dir.isValid() );
  dir.setAutoRemove(true);

  QFile file;
  ByteArraySpan map;
  FileHeader fileHeader;
  std::vector<ProgramHeader> programHeaders;
  std::vector<SectionHeader> sectionHeaders;

  const QString targetFilePath = makePath(dir, "targetFile");
  REQUIRE( copyFile(QString::fromLocal8Bit(TEST_SIMPLE_EXECUTABLE_DYNAMIC_FILE_PATH), targetFilePath) );
  REQUIRE( runExecutable(targetFilePath) );

  map = openAndMapFileForRead(file, targetFilePath);
  REQUIRE( !map.isNull() );

  fileHeader = extractFileHeader(map);
  REQUIRE( fileHeader.seemsValid() );
  programHeaders = extractAllProgramHeaders(map, fileHeader);
  sectionHeaders = extractAllSectionHeaders(map, fileHeader);

  unmapAndCloseFile(file, map);

  map = openAndMapFileForWrite(file, targetFilePath);
  REQUIRE( !map.isNull() );

  unmapAndCloseFile(file, map);
  REQUIRE( runExecutable(targetFilePath) );
  
  REQUIRE( false);
}
