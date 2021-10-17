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
#ifndef ELF_FILE_IO_TEST_UTILS_H
#define ELF_FILE_IO_TEST_UTILS_H

#include "Mdt/DeployUtils/Impl/Elf/FileReader.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include <iostream>
#include <vector>
#include <cassert>

using namespace Mdt::DeployUtils;

Mdt::DeployUtils::Impl::ByteArraySpan arraySpanFromArray(unsigned char * const array, qint64 size)
{
  assert(size > 0);

  Mdt::DeployUtils::Impl::ByteArraySpan span;
  span.data = array;
  span.size = size;

  return span;
}

bool arraysAreEqual(const Mdt::DeployUtils::Impl::ByteArraySpan & array, const Mdt::DeployUtils::Impl::ByteArraySpan & reference)
{
  if(array.size != reference.size){
    std::cout << "sizes differ: " << QString::number(array.size).toStdString()
              << " , expected: " << QString::number(reference.size).toStdString() << std::endl;
    return false;
  }

  for(int64_t i = 0; i < array.size; ++i){
    if(array.data[i] != reference.data[i]){
      std::cout << "values differ at index " << i
      << ": 0x" << QString::number(array.data[i], 16).toStdString()
      << " , extected: 0x" << QString::number(reference.data[i], 16).toStdString() << std::endl;
      return false;
    }
  }

  return true;
//   return std::equal( array.cbegin(), array.cend(), reference.cbegin(), reference.cend() );
}

bool arraysAreEqual(const Mdt::DeployUtils::Impl::ByteArraySpan & array, std::vector<unsigned char> reference)
{
  Mdt::DeployUtils::Impl::ByteArraySpan referenceArray;
  referenceArray.data = reference.data();
  referenceArray.size = reference.size();

  return arraysAreEqual(array, referenceArray);
}

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

Impl::Elf::FileHeader make32BitBigEndianFileHeader()
{
  using Impl::Elf::ObjectFileType;
  using Impl::Elf::Machine;

  Impl::Elf::FileHeader fileHeader;

  fileHeader.ident = make32BitBigEndianIdent();
  fileHeader.setObjectFileType(ObjectFileType::SharedObject);
  fileHeader.setMachineType(Machine::X86);
  fileHeader.version = 1;
  fileHeader.entry = 100;
  fileHeader.phoff = 0x34;
  fileHeader.shoff = 1000;
  fileHeader.flags = 0;
  fileHeader.ehsize = 52;
  fileHeader.phentsize = 32;
  fileHeader.phnum = 9;
  fileHeader.shentsize = 40;
  fileHeader.shnum = 10;
  fileHeader.shstrndx = 9;

  return fileHeader;
}

Impl::Elf::FileHeader make64BitLittleEndianFileHeader()
{
  using Impl::Elf::ObjectFileType;
  using Impl::Elf::Machine;

  Impl::Elf::FileHeader fileHeader;

  fileHeader.ident = make64BitLittleEndianIdent();
  fileHeader.setObjectFileType(ObjectFileType::SharedObject);
  fileHeader.setMachineType(Machine::X86_64);
  fileHeader.version = 1;
  fileHeader.entry = 100;
  fileHeader.phoff = 0x40;
  fileHeader.shoff = 1000;
  fileHeader.flags = 0;
  fileHeader.ehsize = 64;
  fileHeader.phentsize = 56;
  fileHeader.phnum = 9;
  fileHeader.shentsize = 64;
  fileHeader.shnum = 10;
  fileHeader.shstrndx = 9;

  return fileHeader;
}

#endif // #ifndef ELF_FILE_IO_TEST_UTILS_H
