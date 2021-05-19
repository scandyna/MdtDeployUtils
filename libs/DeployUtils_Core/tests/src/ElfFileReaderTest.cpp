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
#include <cassert>

#include <QDebug>

using namespace Mdt::DeployUtils;

Impl::Elf::Ident makeValidIdent(Impl::Elf::Class _class, Impl::Elf::DataFormat dataFormat)
{
  Impl::Elf::Ident ident;

  ident.hasValidElfMagicNumber = true;
  ident._class = _class;
  ident.dataFormat = dataFormat;
  ident.version = 1;
  ident.osabi = 0;
  ident.abiversion = 0;

  assert( ident.isValid() );

  return ident;
}

Impl::Elf::Ident make32BitLittleEndianIdent()
{
  using Impl::Elf::Class;
  using Impl::Elf::DataFormat;

  return makeValidIdent(Class::Class32, DataFormat::Data2LSB);
}

Impl::Elf::Ident make32BitBigEndianIdent()
{
  using Impl::Elf::Class;
  using Impl::Elf::DataFormat;

  return makeValidIdent(Class::Class32, DataFormat::Data2MSB);
}

Impl::Elf::Ident make64BitLittleEndianIdent()
{
  using Impl::Elf::Class;
  using Impl::Elf::DataFormat;

  return makeValidIdent(Class::Class64, DataFormat::Data2LSB);
}

Impl::Elf::Ident make64BitBigEndianIdent()
{
  using Impl::Elf::Class;
  using Impl::Elf::DataFormat;

  return makeValidIdent(Class::Class64, DataFormat::Data2MSB);
}


TEST_CASE("Sandbox")
{
  const QString filePath = QLatin1String("/home/philippe/dev/build/MdtDeployUtils/gccInstrumented/libs/DeployUtils_Core/src/libMdt0DeployUtils.so");
  qDebug() << "Is ELF " << filePath << ": " << ElfFileReader::isElfFile(filePath);

}

TEST_CASE("getAddress")
{
  using Impl::Elf::getAddress;
  using Impl::Elf::Ident;

  Ident ident;

  SECTION("32-bit little-endian")
  {
    ident = make32BitLittleEndianIdent();
    uchar valueArray[4] = {0x78,0x56,0x34,0x12};
    REQUIRE( getAddress(valueArray, ident) == 0x12345678 );
  }

  SECTION("32-bit big-endian")
  {
    ident = make32BitBigEndianIdent();
    uchar valueArray[4] = {0x12,0x34,0x56,0x78};
    REQUIRE( getAddress(valueArray, ident) == 0x12345678 );
  }

  SECTION("64-bit little-endian")
  {
    ident = make64BitLittleEndianIdent();
    uchar valueArray[8] = {0xF0,0xDE,0xBC,0x9A,0x78,0x56,0x34,0x12};
    REQUIRE( getAddress(valueArray, ident) == 0x123456789ABCDEF0 );
  }

  SECTION("64-bit big-endian")
  {
    ident = make64BitBigEndianIdent();
    uchar valueArray[8] = {0x12,0x34,0x56,0x78,0x9A,0xBC,0xDE,0xF0};
    REQUIRE( getAddress(valueArray, ident) == 0x123456789ABCDEF0 );
  }
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

TEST_CASE("e_machine_fromValue")
{
  using Impl::Elf::e_machine_fromValue;
  using Impl::Elf::Machine;

  SECTION("None")
  {
    REQUIRE( e_machine_fromValue(0x00) == Machine::None );
  }

  SECTION("x86")
  {
    REQUIRE( e_machine_fromValue(0x03) == Machine::X86 );
  }

  SECTION("x86-64")
  {
    REQUIRE( e_machine_fromValue(0x3E) == Machine::X86_64 );
  }

  SECTION("unknown, value 0x1234")
  {
    REQUIRE( e_machine_fromValue(0x1234) == Machine::Unknown );
  }
}

TEST_CASE("extract_e_machine")
{
  using Impl::Elf::extract_e_machine;
  using Impl::Elf::Machine;
  using Impl::Elf::DataFormat;

  SECTION("x86-64 little-endian")
  {
    uchar valueArray[2] = {0x3E,0x00};
    REQUIRE( extract_e_machine(valueArray, DataFormat::Data2LSB) == Machine::X86_64 );
  }

  SECTION("x86-64 big-endian")
  {
    uchar valueArray[2] = {0x00,0x3E};
    REQUIRE( extract_e_machine(valueArray, DataFormat::Data2MSB) == Machine::X86_64 );
  }
}

TEST_CASE("extract_e_version")
{
  using Impl::Elf::extract_e_version;
  using Impl::Elf::DataFormat;

  SECTION("1 little-endian")
  {
    uchar valueArray[4] = {1,0,0,0};
    REQUIRE( extract_e_version(valueArray,DataFormat::Data2LSB) == 1 );
  }

  SECTION("1 big-endian")
  {
    uchar valueArray[4] = {0,0,0,1};
    REQUIRE( extract_e_version(valueArray,DataFormat::Data2MSB) == 1 );
  }
}

TEST_CASE("extract_e_entry")
{
  using Impl::Elf::extract_e_entry;
  using Impl::Elf::Ident;

  Ident ident;

  SECTION("32-bit big-endian")
  {
    ident = make32BitBigEndianIdent();
    uchar valueArray[4] = {0x12,0x34,0x56,0x78};
    REQUIRE( extract_e_entry(valueArray, ident) == 0x12345678 );
  }

  SECTION("64-bit little-endian")
  {
    ident = make64BitLittleEndianIdent();
    uchar valueArray[8] = {0x30,0x7E,0x07,0,0,0,0,0};
    REQUIRE( extract_e_entry(valueArray, ident) == 0x77e30 );
  }
}

TEST_CASE("extractFileHeader")
{
  using Impl::Elf::extractFileHeader;
  using Impl::Elf::FileHeader;
  using Impl::Elf::Class;
  using Impl::Elf::DataFormat;
  using Impl::Elf::ObjectFileType;
  using Impl::Elf::Machine;

  FileHeader header;

  SECTION("32-bit big-endian")
  {
    const uchar map[52] = {
      // e_ident
        // magic number
        0x7F,'E','L','F',
        // Class (32 or 64 bit)
        1, // 32
        // Data format
        2,  // big-endian
        // Version
        1,
        // OS ABI
        0x00, // System V
        // ABI version
        0,
        // Padding
        0,0,0,0,0,0,0,
      // e_type
      0,0x03, // ET_DYN (shared object)
      // e_machine
      0,0x03, // x86
      // e_version
      0,0,0,1, // 1
      // e_entry
      0,0,0x32,0x10, // 0x3210
      // e_phoff
      0,0,0,0x34, // 0x34
      // e_shoff
      0,0,0,0, // ???
      // e_flags
      0,0,0,0, // No flags
      // e_ehsize
      0,52, // 52 bytes
      // e_phentsize
      0,56, // 56 bytes
      // e_phnum
      0,7, // 7 entries in program header table
      // e_shentsize
      0,32, // 32 bytes
      // e_shnum
      0,35, // 35 entries in section header table
      // e_shstrndx
      0,34
    };

    header = extractFileHeader(map);
    REQUIRE( header.ident.hasValidElfMagicNumber );
    REQUIRE( header.ident._class == Class::Class32 );
    REQUIRE( header.ident.dataFormat == DataFormat::Data2MSB );
    REQUIRE( header.ident.version == 1 );
    REQUIRE( header.ident.osabi == 0 );
    REQUIRE( header.ident.abiversion == 0 );
    REQUIRE( header.type == ObjectFileType::SharedObject );
    REQUIRE( header.machine == Machine::X86 );
    REQUIRE( header.version == 1 );
    REQUIRE( header.entry == 0x3210 );
    REQUIRE( header.phoff == 0x34 );
  }

  SECTION("64-bit little-endian")
  {
    const uchar map[64] = {
      // e_ident
        // magic number
        0x7F,'E','L','F',
        // Class (32 or 64 bit)
        2, // 64 bit
        // Data format
        1,  // little-endian
        // Version
        1,
        // OS ABI
        0x00, // System V
        // ABI version
        0,
        // Padding
        0,0,0,0,0,0,0,
      // e_type
      0x03,0, // ET_DYN (shared object)
      // e_machine
      0x3E,0, // x86-64
      // e_version
      1,0,0,0, // 1
      // e_entry
      0x10,0x32,0,0,0,0,0,0, // 0x3210
      // e_phoff
      0x40,0,0,0,0,0,0,0, // 0x40
      // e_shoff
      0,0,0,0,0,0,0,0, // ???
      // e_flags
      0,0,0,0, // No flags
      // e_ehsize
      64,0, // 64 bytes
      // e_phentsize
      56,0, // 56 bytes
      // e_phnum
      7,0, // 7 entries in program header table
      // e_shentsize
      32,0, // 32 bytes
      // e_shnum
      35,0, // 35 entries in section header table
      // e_shstrndx
      34,0
    };

    header = extractFileHeader(map);
    REQUIRE( header.ident.hasValidElfMagicNumber );
    REQUIRE( header.ident._class == Class::Class64 );
    REQUIRE( header.ident.dataFormat == DataFormat::Data2LSB );
    REQUIRE( header.ident.version == 1 );
    REQUIRE( header.ident.osabi == 0 );
    REQUIRE( header.ident.abiversion == 0 );
    REQUIRE( header.type == ObjectFileType::SharedObject );
    REQUIRE( header.machine == Machine::X86_64 );
    REQUIRE( header.version == 1 );
    REQUIRE( header.entry == 0x3210 );
    REQUIRE( header.phoff == 0x40 );
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
