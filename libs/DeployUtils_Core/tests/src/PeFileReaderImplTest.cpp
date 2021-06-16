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
#include "Mdt/DeployUtils/Impl/Pe/FileReader.h"

using namespace Mdt::DeployUtils;
using Impl::ByteArraySpan;

ByteArraySpan arraySpanFromArray(const unsigned char * const array, qint64 size)
{
  assert(size > 0);

  Impl::ByteArraySpan span;
  span.data = array;
  span.size = size;

  return span;
}

Impl::Pe::CoffHeader makeValid64BitDllCoffHeader()
{
  Impl::Pe::CoffHeader header;

  header.machine = 0x8664;
  header.characteristics = 0x200e;
  header.sizeOfOptionalHeader = 112;

  return header;
}

Impl::Pe::CoffHeader makeValid32BitDllCoffHeader()
{
  Impl::Pe::CoffHeader header;

  header.machine = 0x14c;
  header.characteristics = 0x230e;
  header.sizeOfOptionalHeader = 112;

  return header;
}


TEST_CASE("DosHeader")
{
  using Impl::Pe::DosHeader;

  SECTION("default constructed")
  {
    DosHeader header;
    REQUIRE( header.isNull() );
    REQUIRE( !header.seemsValid() );
  }
}

TEST_CASE("extractDosHeader")
{
  using Impl::Pe::extractDosHeader;
  using Impl::Pe::DosHeader;
  using Impl::ByteArraySpan;

  uchar map[64] = {};
  DosHeader header;

  SECTION("somewhat valid DOS header")
  {
    map[0] = 'M';
    map[1] = 'Z';
    map[0x3C] = 125;
    map[0x3D] = 0;
    map[0x3E] = 0;
    map[0x3F] = 0;

    ByteArraySpan mapSpan = arraySpanFromArray( map, sizeof(map) );
    header = extractDosHeader(mapSpan);
    REQUIRE( header.peSignatureOffset == 125 );
    REQUIRE( header.seemsValid() );
  }
}

TEST_CASE("containsPeSignature")
{
  using Impl::Pe::containsPeSignature;
  using Impl::Pe::extractDosHeader;
  using Impl::Pe::DosHeader;

  DosHeader dosHeader;
  uchar map[200] = {};
  map[0x3C] = 100;  // PE signature offset

  SECTION("with PE signature")
  {
    map[100] = 'P';
    map[101] = 'E';
    map[102] = 0;
    map[103] = 0;

    ByteArraySpan mapSpan = arraySpanFromArray( map, sizeof(map) );
    dosHeader = extractDosHeader(mapSpan);
    REQUIRE( containsPeSignature(mapSpan, dosHeader) );
  }

  SECTION("no PE signature")
  {
    map[100] = 1;
    map[101] = 2;
    map[102] = 3;
    map[103] = 4;

    ByteArraySpan mapSpan = arraySpanFromArray( map, sizeof(map) );
    dosHeader = extractDosHeader(mapSpan);
    REQUIRE( !containsPeSignature(mapSpan, dosHeader) );
  }

  SECTION("wrong PE signature")
  {
    map[100] = 'P';
    map[101] = 'E';
    map[102] = 0;
    map[103] = 1;

    ByteArraySpan mapSpan = arraySpanFromArray( map, sizeof(map) );
    dosHeader = extractDosHeader(mapSpan);
    REQUIRE( !containsPeSignature(mapSpan, dosHeader) );
  }
}

TEST_CASE("CoffHeader")
{
  using Impl::Pe::CoffHeader;
  using Impl::Pe::MachineType;

  CoffHeader header;

  SECTION("default constructed")
  {
    REQUIRE( header.machineType() == MachineType::Null );
    REQUIRE( !header.seemsValid() );
  }

  SECTION("Machine")
  {
    SECTION("Unknown")
    {
      header.machine = 0x0;
      REQUIRE( header.machineType() == MachineType::Unknown );
    }

    SECTION("AMD64 (x86-64)")
    {
      header.machine = 0x8664;
      REQUIRE( header.machineType() == MachineType::Amd64 );
    }

    SECTION("I386")
    {
      header.machine = 0x14c;
      REQUIRE( header.machineType() == MachineType::I386 );
    }
  }

  SECTION("Characteristics")
  {
    SECTION("Valid 32 bit DLL (release build example)")
    {
      header.characteristics = 0x230e;
      REQUIRE( header.isValidExecutableImage() );
      REQUIRE( header.is32BitWordMachine() );
      REQUIRE( header.isDll() );
    }

    SECTION("Valid 32 bit DLL (debug build example)")
    {
      header.characteristics = 0x2106;
      REQUIRE( header.isValidExecutableImage() );
      REQUIRE( header.is32BitWordMachine() );
      REQUIRE( header.isDll() );
    }

    SECTION("Invalid image")
    {
      header.characteristics = 0x2301;
      REQUIRE( !header.isValidExecutableImage() );
    }
  }
}

TEST_CASE("CoffHeader_seemsValid")
{
  using Impl::Pe::CoffHeader;
  using Impl::Pe::MachineType;

  CoffHeader header;

  SECTION("default constructed")
  {
    REQUIRE( !header.seemsValid() );
  }

  SECTION("Valid 32-bit DLL")
  {
    header.machine = 0x14c;
    header.characteristics = 0x230e;
    header.sizeOfOptionalHeader = 112;
    REQUIRE( header.seemsValid() );
  }

  SECTION("Invalid")
  {
    header = makeValid32BitDllCoffHeader();
    REQUIRE( header.seemsValid() );

    SECTION("Unknown machine")
    {
      header.machine = 0;
      REQUIRE( !header.seemsValid() );
    }

    SECTION("Not a valid executable image")
    {
      header.characteristics = 0;
      REQUIRE( !header.seemsValid() );
    }

    SECTION("SizeOfOptionalHeader not set")
    {
      header.sizeOfOptionalHeader = 0;
      REQUIRE( !header.seemsValid() );
    }

    SECTION("SizeOfOptionalHeader to small")
    {
      header.sizeOfOptionalHeader = 50;
      REQUIRE( !header.seemsValid() );
    }
  }
}

TEST_CASE("coffHeaderFromArray")
{
  using Impl::Pe::coffHeaderFromArray;
  using Impl::Pe::CoffHeader;

  CoffHeader header;

  SECTION("x86_64 (little endian array format)")
  {
    unsigned char array[20] = {
      // Machine
      0x64,0x86, // 0x8664: x64
      // NumberOfSections
      0x23,0x01, // 0x0123
      // TimeDateStamp
      0x90,0x03,0x02,0x01, // 0x01020390
      // PointerToSymbolTable
      0x06,0x05,0x04,0x03,  // 0x03040506
      // NumberOfSymbols
      0x00,0x00,0x00,0x00,  // 0 (NumberOfSymbols is deprecated)
      // SizeOfOptionalHeader
      0x02,0x01, // 0x0102
      // Characteristics
      0x03,0x02 // 0x0203
    };

    ByteArraySpan arraySpan = arraySpanFromArray( array, sizeof(array) );
    header = coffHeaderFromArray(arraySpan);
    REQUIRE( header.machine == 0x8664 );
    REQUIRE( header.numberOfSections == 0x0123 );
    REQUIRE( header.timeDateStamp == 0x01020390 );
    REQUIRE( header.pointerToSymbolTable == 0x03040506 );
    REQUIRE( header.numberOfSymbols == 0 );
    REQUIRE( header.sizeOfOptionalHeader == 0x0102 );
    REQUIRE( header.characteristics == 0x0203 );
  }
}

TEST_CASE("ImageDataDirectory")
{
  using Impl::Pe::ImageDataDirectory;

  ImageDataDirectory directory;

  SECTION("fromUint64")
  {
    directory = ImageDataDirectory::fromUint64(0x1234567887654321);
    REQUIRE( directory.virtualAddress == 0x87654321 );
    REQUIRE( directory.size == 0x12345678 );
  }
}

TEST_CASE("OptionalHeader")
{
  using Impl::Pe::OptionalHeader;
  using Impl::Pe::MagicType;

  OptionalHeader header;

  SECTION("default constructed")
  {
    REQUIRE( header.magicType() == MagicType::Unknown );
  }

  SECTION("Magic type")
  {
    SECTION("PE32"){
      header.magic = 0x10b;
      REQUIRE( header.magicType() == MagicType::Pe32 );
    }

    SECTION("PE32+"){
      header.magic = 0x20b;
      REQUIRE( header.magicType() == MagicType::Pe32Plus );
    }

    SECTION("ROM image"){
      header.magic = 0x107;
      REQUIRE( header.magicType() == MagicType::RomImage );
    }

    SECTION("unknown")
    {
      header.magic = 0x1234;
      REQUIRE( header.magicType() == MagicType::Unknown );
    }
  }

  SECTION("import table")
  {
    SECTION("default constructed")
    {
      REQUIRE( !header.containsImportTable() );
    }

    SECTION("no import table")
    {
      header.numberOfRvaAndSizes = 1;
      REQUIRE( !header.containsImportTable() );
    }

    SECTION("has import table")
    {
      header.numberOfRvaAndSizes = 2;
      header.importTable = 125;
      REQUIRE( header.containsImportTable() );
    }
  }

  SECTION("delay import table")
  {
    SECTION("default constructed")
    {
      REQUIRE( !header.containsDelayImportTable() );
    }

    SECTION("no delay import table")
    {
      header.numberOfRvaAndSizes = 13;
      REQUIRE( !header.containsDelayImportTable() );
    }

    SECTION("has delay import table")
    {
      header.numberOfRvaAndSizes = 14;
      header.delayImportTable = 126;
      REQUIRE( header.containsDelayImportTable() );
    }
  }
}

TEST_CASE("optionalHeaderFromArray")
{
  using Impl::Pe::optionalHeaderFromArray;
  using Impl::Pe::OptionalHeader;
  using Impl::Pe::CoffHeader;

  OptionalHeader header;
  CoffHeader coffHeader;

  SECTION("32-bit image file")
  {
    coffHeader = makeValid32BitDllCoffHeader();
    coffHeader.sizeOfOptionalHeader = 208;

    unsigned char array[208] = {};
    // Magic: 0x010B, PE32
    array[0] = 0x0B;
    array[1] = 0x01;
    // NumberOfRvaAndSizes: 0x12345678
    array[92] = 0x78;
    array[93] = 0x56;
    array[94] = 0x34;
    array[95] = 0x12;
    // Import Table: 0x1234567812345678
    array[104] = 0x78;
    array[105] = 0x56;
    array[106] = 0x34;
    array[107] = 0x12;
    array[108] = 0x78;
    array[109] = 0x56;
    array[110] = 0x34;
    array[111] = 0x12;
    // Delay import table: 0x3456789012345678
    array[200] = 0x78;
    array[201] = 0x56;
    array[202] = 0x34;
    array[203] = 0x12;
    array[204] = 0x90;
    array[205] = 0x78;
    array[206] = 0x56;
    array[207] = 0x34;

    ByteArraySpan arraySpan = arraySpanFromArray( array, sizeof(array) );
    header = optionalHeaderFromArray(arraySpan, coffHeader);
    REQUIRE( header.magic == 0x10B );
    REQUIRE( header.numberOfRvaAndSizes == 0x12345678 );
    REQUIRE( header.importTable == 0x1234567812345678 );
    REQUIRE( header.delayImportTable == 0x3456789012345678 );
  }

  SECTION("64-bit image file")
  {
    coffHeader = makeValid64BitDllCoffHeader();
    coffHeader.sizeOfOptionalHeader = 224;

    unsigned char array[224] = {};
    // Magic: 0x020B, PE32+
    array[0] = 0x0B;
    array[1] = 0x02;
    // NumberOfRvaAndSizes: 0x12345678
    array[108] = 0x78;
    array[109] = 0x56;
    array[110] = 0x34;
    array[111] = 0x12;
    // Import Table: 0x1234567812345678
    array[120] = 0x78;
    array[121] = 0x56;
    array[122] = 0x34;
    array[123] = 0x12;
    array[124] = 0x78;
    array[125] = 0x56;
    array[126] = 0x34;
    array[127] = 0x12;
    // Delay import table: 0x3456789012345678
    array[216] = 0x78;
    array[217] = 0x56;
    array[218] = 0x34;
    array[219] = 0x12;
    array[220] = 0x90;
    array[221] = 0x78;
    array[222] = 0x56;
    array[223] = 0x34;

    ByteArraySpan arraySpan = arraySpanFromArray( array, sizeof(array) );
    header = optionalHeaderFromArray(arraySpan, coffHeader);
    REQUIRE( header.magic == 0x20B );
    REQUIRE( header.numberOfRvaAndSizes == 0x12345678 );
    REQUIRE( header.importTable == 0x1234567812345678 );
    REQUIRE( header.delayImportTable == 0x3456789012345678 );
  }
}

TEST_CASE("qStringFromUft8UnsignedCharArray")
{
  using Impl::Pe::qStringFromUft8BoundedUnsignedCharArray;

  ByteArraySpan arraySpan;

  SECTION("Section table format")
  {
    SECTION(".idata")
    {
      unsigned char array[8] = {
        '.','i','d','a','t','a',0,0
      };
      arraySpan = arraySpanFromArray( array, sizeof(array) );
      REQUIRE( qStringFromUft8BoundedUnsignedCharArray(arraySpan) == QLatin1String(".idata") );
    }

    SECTION("abcdefgh (no string terminating null)")
    {
      unsigned char array[8] = {
        'a','b','c','d','e','f','g','h'
      };
      arraySpan = arraySpanFromArray( array, sizeof(array) );
      REQUIRE( qStringFromUft8BoundedUnsignedCharArray(arraySpan) == QLatin1String("abcdefgh") );
    }
  }
}

TEST_CASE("SectionHeader")
{
  using Impl::Pe::SectionHeader;

  SectionHeader header;

  SECTION("default constructed")
  {
    REQUIRE( !header.seemsValid() );
  }

  SECTION("validity")
  {
    header.name = QLatin1String(".idata");
    header.virtualSize = 10;
    header.virtualAddress = 10000;
    header.sizeOfRawData = 100;
    header.pointerToRawData = 1000;
    REQUIRE( header.seemsValid() );

    SECTION("name with / (not valid for executable image)")
    {
      header.name = QLatin1String("/1234");
      REQUIRE( !header.seemsValid() );
    }

    SECTION("virtualAddress and pointerToRawData")
    {
      SECTION("valid: virtualAddress > pointerToRawData")
      {
        header.virtualAddress = 1000;
        header.pointerToRawData = 100;
        REQUIRE( header.seemsValid() );
      }

      SECTION("valid (limit): virtualAddress == pointerToRawData")
      {
        header.virtualAddress = 1000;
        header.pointerToRawData = 1000;
        REQUIRE( header.seemsValid() );
      }

      SECTION("invalid: virtualAddress < pointerToRawData")
      {
        header.virtualAddress = 100;
        header.pointerToRawData = 1000;
        REQUIRE( !header.seemsValid() );
      }
    }
  }

  SECTION("RVA and file offset")
  {
    header.name = QLatin1String(".idata");
    header.virtualSize = 10;
    header.virtualAddress = 10000;
    header.sizeOfRawData = 100;
    header.pointerToRawData = 1000;
    REQUIRE( header.seemsValid() );

    SECTION("valid RVA")
    {
      REQUIRE( header.rvaIsValid(9500) );
      REQUIRE( header.rvaToFileOffset(9500) == 500 );
    }

    SECTION("valid RVA (limit case)")
    {
      REQUIRE( header.rvaIsValid(9000) );
      REQUIRE( header.rvaToFileOffset(9000) == 0 );
    }

    SECTION("invalid RVA")
    {
      REQUIRE( !header.rvaIsValid(8000) );
    }
  }
}

TEST_CASE("sectionHeaderFromArray")
{
  using Impl::Pe::sectionHeaderFromArray;
  using Impl::Pe::SectionHeader;

  unsigned char array[40] = {};
  ByteArraySpan arraySpan;
  SectionHeader header;

  SECTION(".idata")
  {
    // Name
    array[0] = '.';
    array[1] = 'i';
    array[2] = 'd';
    array[3] = 'a';
    array[4] = 't';
    array[5] = 'a';
    array[6] = 0;
    array[7] = 0;
    // VirtualSize: 0x12345678
    array[8] = 0x78;
    array[9] = 0x56;
    array[10] = 0x34;
    array[11] = 0x12;
    // VirtualAddress: 0x34567890
    array[12] = 0x90;
    array[13] = 0x78;
    array[14] = 0x56;
    array[15] = 0x34;
    // SizeOfRawData: 0x12345678
    array[16] = 0x78;
    array[17] = 0x56;
    array[18] = 0x34;
    array[19] = 0x12;
    // PointerToRawData: 0x87654321
    array[20] = 0x21;
    array[21] = 0x43;
    array[22] = 0x65;
    array[23] = 0x87;

    arraySpan = arraySpanFromArray( array, sizeof(array) );
    header = sectionHeaderFromArray(arraySpan);
    REQUIRE( header.name == QLatin1String(".idata") );
    REQUIRE( header.virtualSize == 0x12345678 );
    REQUIRE( header.virtualAddress == 0x34567890 );
    REQUIRE( header.sizeOfRawData == 0x12345678 );
    REQUIRE( header.pointerToRawData == 0x87654321 );
  }
}

TEST_CASE("importDirectoryFromArray")
{
  using Impl::Pe::importDirectoryFromArray;
  using Impl::Pe::ImportDirectory;

  unsigned char array[20] = {};
  ByteArraySpan arraySpan;
  ImportDirectory directory;

  // Name RVA: 0x12345678
  array[12] = 0x78;
  array[13] = 0x56;
  array[14] = 0x34;
  array[15] = 0x12;

  arraySpan = arraySpanFromArray( array, sizeof(array) );
  directory = importDirectoryFromArray(arraySpan);
  REQUIRE( directory.nameRVA == 0x12345678 );
}

TEST_CASE("delayLoadDirectoryFromArray")
{
  using Impl::Pe::delayLoadDirectoryFromArray;
  using Impl::Pe::DelayLoadDirectory;

  unsigned char array[32] = {};
  ByteArraySpan arraySpan;
  DelayLoadDirectory directory;

  // Name RVA: 0x12345678
  array[4] = 0x78;
  array[5] = 0x56;
  array[6] = 0x34;
  array[7] = 0x12;

  arraySpan = arraySpanFromArray( array, sizeof(array) );
  directory = delayLoadDirectoryFromArray(arraySpan);
  REQUIRE( directory.nameRVA == 0x12345678 );
}
