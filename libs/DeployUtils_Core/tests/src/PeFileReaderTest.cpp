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
#include "Mdt/DeployUtils/PeFileReader.h"
#include <QString>
#include <QLatin1String>
#include <QTemporaryFile>

#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include "Mdt/DeployUtils/Impl/FileMapper.h"
#include <QFile>
#include <QDebug>

using namespace Mdt::DeployUtils;

TEST_CASE("sandbox")
{
  using Impl::ByteArraySpan;
  using Impl::FileMapper;

  QFile file;
  file.setFileName( QLatin1String("/home/philippe/.wine/drive_c/Qt/Qt5.6.2/5.6/mingw49_32/bin/Qt5Cored.dll") );
  REQUIRE( file.open(QIODevice::ReadOnly) );

  FileMapper mapper;
  const ByteArraySpan map = mapper.mapIfRequired( file, 0, file.size() );
  
  qDebug() << "file size: " << map.size;
  
}

TEST_CASE("open_close")
{
  QTemporaryFile file;
  REQUIRE( file.open() );

  PeFileReader reader;
  REQUIRE( !reader.isOpen() );

  SECTION("empty file")
  {
    reader.openFile( file.fileName() );
    REQUIRE( reader.isOpen() );
    reader.close();
    REQUIRE( !reader.isOpen() );
  }
}

