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
#include "Mdt/DeployUtils/Impl/FileMapper.h"
#include <QTemporaryFile>
#include <QLatin1String>
#include <QString>

// #include <QDebug>

using namespace Mdt::DeployUtils;
using Impl::FileMapper;
using Impl::ByteArraySpan;


TEST_CASE("needToRemap")
{
  QTemporaryFile file;
  REQUIRE( file.open() );
  REQUIRE( writeTextFileUtf8( file, QLatin1String("abcdefghijklmnopqrstuvwxyz") ) );
  REQUIRE( file.flush() );

  FileMapper mapper;

  SECTION("just after construction (initial)")
  {
    REQUIRE( mapper.needToRemap(0, 1) );
  }

  SECTION("2x same offset and size")
  {
    mapper.mapIfRequired(file, 0, 1);
    REQUIRE( !mapper.needToRemap(0, 1) );
  }

  SECTION("different offset")
  {
    mapper.mapIfRequired(file, 1, 1);
    REQUIRE( mapper.needToRemap(0, 1) );
  }

  SECTION("size 1 then 2")
  {
    mapper.mapIfRequired(file, 0, 1);
    REQUIRE( mapper.needToRemap(0, 2) );
  }

  SECTION("size 2 then 1")
  {
    mapper.mapIfRequired(file, 0, 2);
    REQUIRE( !mapper.needToRemap(0, 1) );
  }
}

TEST_CASE("mapIfRequired")
{
  QTemporaryFile file;
  REQUIRE( file.open() );
  REQUIRE( writeTextFileUtf8( file, QLatin1String("abcdefghijklmnopqrstuvwxyz") ) );
  REQUIRE( file.flush() );

  FileMapper mapper;
  ByteArraySpan map;

  SECTION("map a")
  {
    map = mapper.mapIfRequired(file, 0, 1);
    REQUIRE( map.size == 1 );
    REQUIRE( map.data[0] == 'a' );
  }

  SECTION("map bc")
  {
    map = mapper.mapIfRequired(file, 1, 2);
    REQUIRE( map.size == 2 );
    REQUIRE( map.data[0] == 'b' );
    REQUIRE( map.data[1] == 'c' );
  }

  SECTION("map ab then abcd then bc")
  {
    map = mapper.mapIfRequired(file, 0, 2);
    REQUIRE( map.size == 2 );
    REQUIRE( map.data[0] == 'a' );
    REQUIRE( map.data[1] == 'b' );

    map = mapper.mapIfRequired(file, 0, 4);
    REQUIRE( map.size == 4 );
    REQUIRE( map.data[0] == 'a' );
    REQUIRE( map.data[1] == 'b' );
    REQUIRE( map.data[2] == 'c' );
    REQUIRE( map.data[3] == 'd' );

    map = mapper.mapIfRequired(file, 1, 2);
    REQUIRE( map.size == 2 );
    REQUIRE( map.data[0] == 'b' );
    REQUIRE( map.data[1] == 'c' );
  }

  SECTION("map abc then ab (remap not required)")
  {
    map = mapper.mapIfRequired(file, 0, 3);
    REQUIRE( map.size == 3 );
    REQUIRE( map.data[0] == 'a' );
    REQUIRE( map.data[1] == 'b' );
    REQUIRE( map.data[2] == 'c' );

    map = mapper.mapIfRequired(file, 0, 2);
    REQUIRE( map.size == 2 );
    REQUIRE( map.data[0] == 'a' );
    REQUIRE( map.data[1] == 'b' );
  }

  SECTION("map the entire file")
  {
    map = mapper.mapIfRequired( file, 0, file.size() );
    REQUIRE( map.size == file.size() );
    REQUIRE( map.data[0] == 'a' );
  }
}

TEST_CASE("unmap")
{
  QTemporaryFile file;
  REQUIRE( file.open() );
  REQUIRE( writeTextFileUtf8( file, QLatin1String("abcdefghijklmnopqrstuvwxyz") ) );
  REQUIRE( file.flush() );

  FileMapper mapper;

  SECTION("just after construction (nothing mapped)")
  {
    mapper.unmap(file);
  }

  SECTION("closed file")
  {
    file.close();
    mapper.unmap(file);
  }

  SECTION("mapped file")
  {
    const ByteArraySpan map = mapper.mapIfRequired(file, 0, 5);
    REQUIRE( map.size == 5 );
    mapper.unmap(file);
  }
}

TEST_CASE("mapIfRequired_2_files_with_same_instance")
{
  QTemporaryFile file1;
  REQUIRE( file1.open() );
  REQUIRE( writeTextFileUtf8( file1, QLatin1String("abcd") ) );
  REQUIRE( file1.flush() );

  QTemporaryFile file2;
  REQUIRE( file2.open() );
  REQUIRE( writeTextFileUtf8( file2, QLatin1String("efgh") ) );
  REQUIRE( file2.flush() );

  FileMapper mapper;
  ByteArraySpan map;

  map = mapper.mapIfRequired(file1, 0, 2);
  REQUIRE( map.size == 2 );
  REQUIRE( map.data[0] == 'a' );
  REQUIRE( map.data[1] == 'b' );
  mapper.unmap(file1);
  file1.close();

  map = mapper.mapIfRequired(file2, 0, 2);
  REQUIRE( map.size == 2 );
  REQUIRE( map.data[0] == 'e' );
  REQUIRE( map.data[1] == 'f' );
  mapper.unmap(file2);
  file2.close();
}
