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
#include "Mdt/DeployUtils/ElfFileReader.h"
#include "Mdt/DeployUtils/Impl/Elf/ElfFileReader.h"
#include <QString>
#include <QLatin1String>
#include <QTemporaryFile>

#include <QDebug>

using namespace Mdt::DeployUtils;

TEST_CASE("Sandbox")
{
  const QString filePath = QLatin1String("/home/philippe/dev/build/MdtDeployUtils/gccInstrumented/libs/DeployUtils_Core/src/libMdt0DeployUtils.so");
  qDebug() << "Is ELF " << filePath << ": " << ElfFileReader::isElfFile(filePath);

}

TEST_CASE("Ident")
{
  using Impl::Elf::Class;
  using Impl::Elf::DataFormat;

  Impl::Elf::Ident ident;

  SECTION("Valid")
  {
    ident.hasValidElfMagicNumber = true;
    ident._class = Class::Class64;
    ident.dataFormat = DataFormat::Data2LSB;
    ident.version = 1;
    ident.osabi = 0;
    REQUIRE( ident.isValid() );
  }

  SECTION("invalid")
  {
    ident.hasValidElfMagicNumber = true;
    ident._class = Class::Class64;
    ident.dataFormat = DataFormat::Data2LSB;
    ident.version = 1;
    ident.osabi = 0;
    REQUIRE( ident.isValid() );

    SECTION("invalid magic number")
    {
      ident.hasValidElfMagicNumber = false;
      REQUIRE( !ident.isValid() );
    }

    SECTION("unknown class")
    {
      ident._class = Class::ClassNone;
      REQUIRE( !ident.isValid() );
    }

    SECTION("unknown data format")
    {
      ident.dataFormat = DataFormat::DataNone;
      REQUIRE( !ident.isValid() );
    }

    SECTION("invalid version")
    {
      ident.version = 0;
      REQUIRE( !ident.isValid() );
    }

    SECTION("invalid os ABI")
    {
      SECTION("15")
      {
        ident.osabi = 15;
        REQUIRE( !ident.isValid() );
      }

      SECTION("63")
      {
        ident.osabi = 63;
        REQUIRE( !ident.isValid() );
      }
    }
  }
}

TEST_CASE("identClassFromByte")
{
  using Impl::Elf::identClassFromByte;
  using Impl::Elf::Class;

  SECTION("32 bit")
  {
    REQUIRE( identClassFromByte(1) == Class::Class32 );
  }

  SECTION("64 bit")
  {
    REQUIRE( identClassFromByte(2) == Class::Class64 );
  }

  SECTION("invalid value 0")
  {
    REQUIRE( identClassFromByte(0) == Class::ClassNone );
  }

  SECTION("invalid value 255")
  {
    REQUIRE( identClassFromByte(255) == Class::ClassNone );
  }
}

TEST_CASE("dataFormatFromByte")
{
  using Impl::Elf::dataFormatFromByte;
  using Impl::Elf::DataFormat;

  SECTION("little-endian")
  {
    REQUIRE( dataFormatFromByte(1) == DataFormat::Data2LSB );
  }

  SECTION("big-endian")
  {
    REQUIRE( dataFormatFromByte(2) == DataFormat::Data2MSB );
  }

  SECTION("invalid value 0")
  {
    REQUIRE( dataFormatFromByte(0) == DataFormat::DataNone );
  }

  SECTION("invalid value 255")
  {
    REQUIRE( dataFormatFromByte(255) == DataFormat::DataNone );
  }
}

TEST_CASE("extractIdent")
{
  using Impl::Elf::extractIdent;
  using Impl::Elf::Class;
  using Impl::Elf::DataFormat;

  Impl::Elf::Ident ident;

  SECTION("valid 32 bit Linux")
  {
    uchar map[16] = {
      // magic number
      0x7F,'E','L','F',
      // Class (32 or 64 bit)
      1, // 32
      // Data format
      2,  // big-endian
      // Version
      1,
      // OS ABI
      0x03, // Linux
      // ABI version
      1,
      // Padding
      0,0,0,0,0,0,0
    };
    ident = extractIdent(map);
    REQUIRE( ident.hasValidElfMagicNumber );
    REQUIRE( ident._class == Class::Class32 );
    REQUIRE( ident.dataFormat == DataFormat::Data2MSB );
    REQUIRE( ident.version == 1 );
    REQUIRE( ident.osabi == 0x03 );
    REQUIRE( ident.abiversion == 1 );
  }
}

TEST_CASE("e_type_fromValue")
{
  using Impl::Elf::e_type_fromValue;
  using Impl::Elf::ObjectFileType;

  SECTION("None")
  {
    REQUIRE( e_type_fromValue(0) == ObjectFileType::None );
  }

  SECTION("RelocatableFile")
  {
    REQUIRE( e_type_fromValue(0x01) == ObjectFileType::RelocatableFile );
  }

  SECTION("ExecutableFile")
  {
    REQUIRE( e_type_fromValue(0x02) == ObjectFileType::ExecutableFile );
  }

  SECTION("SharedObject")
  {
    REQUIRE( e_type_fromValue(0x03) == ObjectFileType::SharedObject );
  }

  SECTION("CoreFile")
  {
    REQUIRE( e_type_fromValue(0x04) == ObjectFileType::CoreFile );
  }

  SECTION("Unknown 0x05")
  {
    REQUIRE( e_type_fromValue(0x05) == ObjectFileType::Unknown );
  }

  SECTION("Unknown 0x1FFF")
  {
    REQUIRE( e_type_fromValue(0x1FFF) == ObjectFileType::Unknown );
  }
}

TEST_CASE("extract_e_type")
{
  using Impl::Elf::extract_e_type;
  using Impl::Elf::ObjectFileType;
  using Impl::Elf::DataFormat;

  SECTION("SharedObject little-endian")
  {
    uchar valueArray[2] = {0x03,0x00};
    REQUIRE( extract_e_type(valueArray, DataFormat::Data2LSB) == ObjectFileType::SharedObject );
  }

  SECTION("SharedObject big-endian")
  {
    uchar valueArray[2] = {0x00,0x03};
    REQUIRE( extract_e_type(valueArray, DataFormat::Data2MSB) == ObjectFileType::SharedObject );
  }
}

TEST_CASE("isElfFile")
{
  QTemporaryFile file;
  REQUIRE( file.open() );

  SECTION("empty file")
  {
    REQUIRE( !ElfFileReader::isElfFile( file.fileName() ) );
  }

  SECTION("text file - 3 chars")
  {
    REQUIRE( writeTextFileUtf8( file, QLatin1String("ABC") ) );
    file.close();
    REQUIRE( !ElfFileReader::isElfFile( file.fileName() ) );
  }

  SECTION("text file - 4 chars")
  {
    REQUIRE( writeTextFileUtf8( file, QLatin1String("ABCD") ) );
    file.close();
    REQUIRE( !ElfFileReader::isElfFile( file.fileName() ) );
  }

  SECTION("text file")
  {
    REQUIRE( writeTextFileUtf8( file, QLatin1String("ABCDEFGHIJKLMNOPQRSTUWXYZ") ) );
    file.close();
    REQUIRE( !ElfFileReader::isElfFile( file.fileName() ) );
  }

}
