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
#include "ElfFileIoImplTestCommon.h"
#include "TestFileUtils.h"
#include "ElfFileIoTestUtils.h"
#include "Mdt/DeployUtils/Impl/Elf/FileReader.h"
#include <QString>
#include <QLatin1String>
#include <QTemporaryFile>
#include <cassert>

// #include <QDebug>

using namespace Mdt::DeployUtils;


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

TEST_CASE("getSignedNWord")
{
  using Impl::Elf::getSignedNWord;
  using Impl::Elf::Ident;

  Ident ident;

  SECTION("32-bit little-endian")
  {
    ident = make32BitLittleEndianIdent();
    uchar valueArray[4] = {0x78,0x56,0x34,0x12};
    REQUIRE( getSignedNWord(valueArray, ident) == 0x12345678 );
  }

  SECTION("32-bit big-endian")
  {
    ident = make32BitBigEndianIdent();
    uchar valueArray[4] = {0x12,0x34,0x56,0x78};
    REQUIRE( getSignedNWord(valueArray, ident) == 0x12345678 );
  }

  SECTION("64-bit little-endian")
  {
    ident = make64BitLittleEndianIdent();
    uchar valueArray[8] = {0xF0,0xDE,0xBC,0x9A,0x78,0x56,0x34,0x12};
    REQUIRE( getSignedNWord(valueArray, ident) == 0x123456789ABCDEF0 );
  }

  SECTION("64-bit big-endian")
  {
    ident = make64BitBigEndianIdent();
    uchar valueArray[8] = {0x12,0x34,0x56,0x78,0x9A,0xBC,0xDE,0xF0};
    REQUIRE( getSignedNWord(valueArray, ident) == 0x123456789ABCDEF0 );
  }
}

TEST_CASE("nextPositionAfterAddress")
{
  using Impl::Elf::nextPositionAfterAddress;
  using Impl::Elf::Ident;

  const unsigned char map[8] = {};
  const unsigned char * it;
  Ident ident;

  SECTION("32-bit")
  {
    ident = make32BitBigEndianIdent();
    it = nextPositionAfterAddress(map, ident);
    REQUIRE( it == map + 4 );
  }

  SECTION("32-bit")
  {
    ident = make64BitLittleEndianIdent();
    it = nextPositionAfterAddress(map, ident);
    REQUIRE( it == map + 8 );
  }
}

TEST_CASE("advance4or8bytes")
{
  using Impl::Elf::advance4or8bytes;
  using Impl::Elf::Ident;

  const unsigned char map[8] = {};
  const unsigned char * it = map;
  Ident ident;

  SECTION("32-bit")
  {
    ident = make32BitBigEndianIdent();
    advance4or8bytes(it, ident);
    REQUIRE( it == map + 4 );
  }

  SECTION("32-bit")
  {
    ident = make64BitLittleEndianIdent();
    advance4or8bytes(it, ident);
    REQUIRE( it == map + 8 );
  }
}

TEST_CASE("Ident")
{
  using Impl::Elf::Class;
  using Impl::Elf::DataFormat;

  Impl::Elf::Ident ident;

  SECTION("default constructed")
  {
    REQUIRE( !ident.isValid() );
  }

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

    SECTION("clear")
    {
      ident.clear();
      REQUIRE( !ident.isValid() );
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
  using Impl::ByteArraySpan;

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
    ByteArraySpan mapSpan = arraySpanFromArray( map, sizeof(map) );
    ident = extractIdent(mapSpan);
    REQUIRE( ident.hasValidElfMagicNumber );
    REQUIRE( ident._class == Class::Class32 );
    REQUIRE( ident.dataFormat == DataFormat::Data2MSB );
    REQUIRE( ident.version == 1 );
    REQUIRE( ident.osabi == 0x03 );
    REQUIRE( ident.abiversion == 1 );
  }
}

TEST_CASE("fileHeader_objectFileType")
{
  using Impl::Elf::FileHeader;
  using Impl::Elf::ObjectFileType;

  FileHeader fileHeader;

  SECTION("default constructed")
  {
    REQUIRE( fileHeader.objectFileType() == ObjectFileType::None );
  }

  SECTION("RelocatableFile")
  {
    fileHeader.type = 0x01;
    REQUIRE( fileHeader.objectFileType() == ObjectFileType::RelocatableFile );
  }

  SECTION("ExecutableFile")
  {
    fileHeader.type = 0x02;
    REQUIRE( fileHeader.objectFileType() == ObjectFileType::ExecutableFile );
  }

  SECTION("SharedObject")
  {
    fileHeader.type = 0x03;
    REQUIRE( fileHeader.objectFileType() == ObjectFileType::SharedObject );
  }

  SECTION("CoreFile")
  {
    fileHeader.type = 0x04;
    REQUIRE( fileHeader.objectFileType() == ObjectFileType::CoreFile );
  }

  SECTION("Unknown 0x05")
  {
    fileHeader.type = 0x05;
    REQUIRE( fileHeader.objectFileType() == ObjectFileType::Unknown );
  }

  SECTION("Unknown 0x1FFF")
  {
    fileHeader.type = 0x1FFF;
    REQUIRE( fileHeader.objectFileType() == ObjectFileType::Unknown );
  }
}

TEST_CASE("fileHeader_setObjectFileType")
{
  using Impl::Elf::FileHeader;
  using Impl::Elf::ObjectFileType;

  FileHeader fileHeader;

  SECTION("SharedObject")
  {
    fileHeader.setObjectFileType(ObjectFileType::SharedObject);
    REQUIRE( fileHeader.type == 0x03 );
    REQUIRE( fileHeader.objectFileType() == ObjectFileType::SharedObject );
  }
}

TEST_CASE("extract_e_type")
{
  using Impl::Elf::extract_e_type;
  using Impl::Elf::DataFormat;

  SECTION("SharedObject little-endian")
  {
    uchar valueArray[2] = {0x03,0x00};
    REQUIRE( extract_e_type(valueArray, DataFormat::Data2LSB) == 0x03 );
  }

  SECTION("SharedObject big-endian")
  {
    uchar valueArray[2] = {0x00,0x03};
    REQUIRE( extract_e_type(valueArray, DataFormat::Data2MSB) == 0x03 );
  }
}

TEST_CASE("fileHeader_machineType")
{
  using Impl::Elf::FileHeader;
  using Impl::Elf::Machine;

  FileHeader fileHeader;

  SECTION("None")
  {
    fileHeader.machine = 0x00;
    REQUIRE( fileHeader.machineType() == Machine::None );
  }

  SECTION("x86")
  {
    fileHeader.machine = 0x03;
    REQUIRE( fileHeader.machineType() == Machine::X86 );
  }

  SECTION("x86-64")
  {
    fileHeader.machine = 0x3E;
    REQUIRE( fileHeader.machineType() == Machine::X86_64 );
  }

  SECTION("unknown, value 0x1234")
  {
    fileHeader.machine = 0x1234;
    REQUIRE( fileHeader.machineType() == Machine::Unknown );
  }
}

TEST_CASE("fileHeader_setMachineType")
{
  using Impl::Elf::FileHeader;
  using Impl::Elf::Machine;

  FileHeader fileHeader;

  SECTION("x86-64")
  {
    fileHeader.setMachineType(Machine::X86_64);
    REQUIRE( fileHeader.machine == 0x3E );
    REQUIRE( fileHeader.machineType() == Machine::X86_64 );
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
    REQUIRE( extract_e_machine(valueArray, DataFormat::Data2LSB) == 0x3E );
  }

  SECTION("x86-64 big-endian")
  {
    uchar valueArray[2] = {0x00,0x3E};
    REQUIRE( extract_e_machine(valueArray, DataFormat::Data2MSB) == 0x3E );
  }
}

TEST_CASE("fileHeader_seemsValid")
{
  using Impl::Elf::FileHeader;
  using Impl::Elf::ObjectFileType;
  using Impl::Elf::Machine;

  FileHeader fileHeader;
  fileHeader.ident = make64BitLittleEndianIdent();
  fileHeader.setObjectFileType(ObjectFileType::SharedObject);
  fileHeader.setMachineType(Machine::X86_64);
  fileHeader.version = 1;
  REQUIRE( fileHeader.seemsValid() );

  SECTION("default constructed")
  {
    FileHeader defaultFileHeader;
    REQUIRE( !defaultFileHeader.seemsValid() );
  }

  SECTION("e_type: ET_NONE")
  {
    fileHeader.setObjectFileType(ObjectFileType::None);
    REQUIRE( !fileHeader.seemsValid() );
  }

  SECTION("e_machine: ET_NONE")
  {
    fileHeader.setMachineType(Machine::None);
    REQUIRE( !fileHeader.seemsValid() );
  }

  SECTION("e_machine: Unknown")
  {
    fileHeader.setMachineType(Machine::Unknown);
    REQUIRE( !fileHeader.seemsValid() );
  }

  SECTION("e_version: EV_NONE")
  {
    fileHeader.version = 0;
    REQUIRE( !fileHeader.seemsValid() );
  }

  SECTION("clear")
  {
    fileHeader.clear();
    REQUIRE( !fileHeader.seemsValid() );
  }
}

TEST_CASE("fileHeader_minimumSizeToReadAllProgramHeaders")
{
  using Impl::Elf::FileHeader;

  FileHeader fileHeader = make64BitLittleEndianFileHeader();
  fileHeader.phoff = 64;
  fileHeader.phentsize = 56;
  fileHeader.phnum = 7;

  const int64_t expectedSize = 64 + 7*56;
  REQUIRE( fileHeader.minimumSizeToReadAllProgramHeaders() == expectedSize );
}

TEST_CASE("fileHeader_minimumSizeToReadAllSectionHeaders()")
{
  using Impl::Elf::FileHeader;

  FileHeader fileHeader = make64BitLittleEndianFileHeader();
  fileHeader.shoff = 1000;
  fileHeader.shentsize = 64;
  fileHeader.shnum = 10;

  const qint64 expectedSize = 1000 + 10*64;
  REQUIRE( fileHeader.minimumSizeToReadAllSectionHeaders() == expectedSize );
}

TEST_CASE("minimumSizeToReadFileHeader")
{
  using Impl::Elf::minimumSizeToReadFileHeader;
  using Impl::Elf::Ident;

  Ident ident;

  SECTION("32-bit")
  {
    ident = make32BitBigEndianIdent();
    REQUIRE( minimumSizeToReadFileHeader(ident) == 52 );
  }

  SECTION("64-bit")
  {
    ident = make64BitLittleEndianIdent();
    REQUIRE( minimumSizeToReadFileHeader(ident) == 64 );
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
  using Impl::ByteArraySpan;

  FileHeader header;

  SECTION("32-bit big-endian")
  {
    uchar map[52] = {
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
      0x12,0x34,0x56,0x78, // 0x12345678
      // e_flags
      0,0,0,0x12, // Some flags
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

    ByteArraySpan mapSpan = arraySpanFromArray( map, sizeof(map) );
    header = extractFileHeader(mapSpan);
    REQUIRE( header.ident.hasValidElfMagicNumber );
    REQUIRE( header.ident._class == Class::Class32 );
    REQUIRE( header.ident.dataFormat == DataFormat::Data2MSB );
    REQUIRE( header.ident.version == 1 );
    REQUIRE( header.ident.osabi == 0 );
    REQUIRE( header.ident.abiversion == 0 );
    REQUIRE( header.objectFileType() == ObjectFileType::SharedObject );
    REQUIRE( header.machineType() == Machine::X86 );
    REQUIRE( header.version == 1 );
    REQUIRE( header.entry == 0x3210 );
    REQUIRE( header.phoff == 0x34 );
    REQUIRE( header.shoff == 0x12345678 );
    REQUIRE( header.flags == 0x12 );
    REQUIRE( header.ehsize == 52 );
    REQUIRE( header.phentsize == 56 );
    REQUIRE( header.phnum == 7 );
    REQUIRE( header.shentsize == 32 );
    REQUIRE( header.shnum == 35 );
    REQUIRE( header.shstrndx == 34 );
  }

  SECTION("64-bit little-endian")
  {
    uchar map[64] = {
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
      0x90,0x78,0x56,0x34,0x12,0,0,0, // 0x1234567890
      // e_flags
      0x12,0,0,0, // Some flags
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

    ByteArraySpan mapSpan = arraySpanFromArray( map, sizeof(map) );
    header = extractFileHeader(mapSpan);
    REQUIRE( header.ident.hasValidElfMagicNumber );
    REQUIRE( header.ident._class == Class::Class64 );
    REQUIRE( header.ident.dataFormat == DataFormat::Data2LSB );
    REQUIRE( header.ident.version == 1 );
    REQUIRE( header.ident.osabi == 0 );
    REQUIRE( header.ident.abiversion == 0 );
    REQUIRE( header.objectFileType() == ObjectFileType::SharedObject );
    REQUIRE( header.machineType() == Machine::X86_64 );
    REQUIRE( header.version == 1 );
    REQUIRE( header.entry == 0x3210 );
    REQUIRE( header.phoff == 0x40 );
    REQUIRE( header.shoff == 0x1234567890 );
    REQUIRE( header.flags == 0x12 );
    REQUIRE( header.ehsize == 64 );
    REQUIRE( header.phentsize == 56 );
    REQUIRE( header.phnum == 7 );
    REQUIRE( header.shentsize == 32 );
    REQUIRE( header.shnum == 35 );
    REQUIRE( header.shstrndx == 34 );
  }
}

TEST_CASE("stringFromUnsignedCharArray")
{
  using Impl::Elf::stringFromUnsignedCharArray;
  using Impl::ByteArraySpan;

  ByteArraySpan span;

  SECTION("A")
  {
    unsigned char array[2] = {'A','\0'};
    span.data = array;
    span.size = 2;
    REQUIRE( stringFromUnsignedCharArray(span) == "A" );
  }
}

TEST_CASE("SectionHeaderType")
{
  using Impl::Elf::SectionHeader;
  using Impl::Elf::SectionType;

  SectionHeader header;

  SECTION("Null")
  {
    header.type = 0;
    REQUIRE( header.sectionType() == SectionType::Null );
  }

  SECTION("StringTable")
  {
    header.type = 3;
    REQUIRE( header.sectionType() == SectionType::StringTable );
  }
}

TEST_CASE("sectionHeaderArraySizeIsBigEnough")
{
  using Impl::Elf::sectionHeaderArraySizeIsBigEnough;
  using Impl::Elf::SectionHeader;
  using Impl::Elf::FileHeader;
  using Impl::ByteArraySpan;

  FileHeader fileHeader;

  ByteArraySpan array;
  unsigned char arrayData[1] = {}; // data is not important here
  array.data = arrayData;

  SECTION("32-bit")
  {
    fileHeader = make32BitBigEndianFileHeader();

    SECTION("to small (39 bytes)")
    {
      array.size = 39;
      REQUIRE( !sectionHeaderArraySizeIsBigEnough(array, fileHeader) );
    }

    SECTION("ok (40 bytes)")
    {
      array.size = 40;
      REQUIRE( sectionHeaderArraySizeIsBigEnough(array, fileHeader) );
    }
  }

  SECTION("64-bit")
  {
    fileHeader = make64BitLittleEndianFileHeader();

    SECTION("to small (63 bytes)")
    {
      array.size = 63;
      REQUIRE( !sectionHeaderArraySizeIsBigEnough(array, fileHeader) );
    }

    SECTION("ok (64 bytes)")
    {
      array.size = 64;
      REQUIRE( sectionHeaderArraySizeIsBigEnough(array, fileHeader) );
    }
  }
}

TEST_CASE("sectionHeaderFromArray")
{
  using Impl::Elf::sectionHeaderFromArray;
  using Impl::Elf::SectionHeader;
  using Impl::Elf::FileHeader;
  using Impl::ByteArraySpan;

  SectionHeader sectionHeader;
  FileHeader fileHeader;
  ByteArraySpan array;

  SECTION("32-bit big-endian")
  {
    uchar sectionHeaderArray[40] = {
      // sh_name
      0,0,0x12,0x34, // 0x1234
      // sh_type
      0,0,0,0x03, // String table
      // sh_flags
      0,0,0,0x20, // Contains null-terminated strings
      // sh_addr
      0x12,0x34,0x56,0x78, // 0x12345678
      // sh_offset
      0,0,0x91,0x23, // 0x9123
      // sh_size
      0,0,0x12,0x34, // 0x1234
      // sh_link
      0,0,0,0x56, // 0x56
      // sh_info
      0x23,0x45,0x67,0x89, // 0x23456789
      // sh_addralign
      0x34,0x56,0x78,0x90, // 0x34567890
      // sh_entsize
      0x45,0x67,0x89,0x01  // 0x45678901
    };

    fileHeader = make32BitBigEndianFileHeader();
    array.data = sectionHeaderArray;
    array.size = sizeof(sectionHeaderArray);
    sectionHeader = sectionHeaderFromArray(array, fileHeader);
    REQUIRE( sectionHeader.nameIndex == 0x1234 );
    REQUIRE( sectionHeader.type == 0x03 );
    REQUIRE( sectionHeader.flags == 0x20 );
    REQUIRE( sectionHeader.addr == 0x12345678 );
    REQUIRE( sectionHeader.offset == 0x9123 );
    REQUIRE( sectionHeader.size == 0x1234 );
    REQUIRE( sectionHeader.link == 0x56 );
    REQUIRE( sectionHeader.info == 0x23456789 );
    REQUIRE( sectionHeader.addralign == 0x34567890 );
    REQUIRE( sectionHeader.entsize == 0x45678901 );
  }

  SECTION("64-bit little-endian")
  {
    uchar sectionHeaderArray[64] = {
      // sh_name
      0x34,0x12,0,0, // 0x1234
      // sh_type
      0x03,0,0,0, // String table
      // sh_flags
      0x20,0,0,0,0,0,0,0, // Contains null-terminated strings
      // sh_addr
      0x78,0x56,0x34,0x12,0,0,0,0, // 0x12345678
      // sh_offset
      0x23,0x91,0,0,0,0,0,0, // 0x9123
      // sh_size
      0x34,0x12,0,0,0,0,0,0, // 0x1234
      // sh_link
      0x56,0,0,0, // 0x56
      // sh_info
      0x89,0x67,0x45,0x23, // 0x23456789
      // sh_addralign
      0x90,0x78,0x56,0x34,0,0,0,0, // 0x34567890
      // sh_entsize
      0x01,0x89,0x67,0x45,0,0,0,0  // 0x45678901
    };

    fileHeader = make64BitLittleEndianFileHeader();
    array.data = sectionHeaderArray;
    array.size = sizeof(sectionHeaderArray);
    sectionHeader = sectionHeaderFromArray(array, fileHeader);
    REQUIRE( sectionHeader.nameIndex == 0x1234 );
    REQUIRE( sectionHeader.type == 0x03 );
    REQUIRE( sectionHeader.flags == 0x20 );
    REQUIRE( sectionHeader.addr == 0x12345678 );
    REQUIRE( sectionHeader.offset == 0x9123 );
    REQUIRE( sectionHeader.size == 0x1234 );
    REQUIRE( sectionHeader.link == 0x56 );
    REQUIRE( sectionHeader.info == 0x23456789 );
    REQUIRE( sectionHeader.addralign == 0x34567890 );
    REQUIRE( sectionHeader.entsize == 0x45678901 );
  }
}
