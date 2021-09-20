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
#include "ElfFileIoImplTestCommon.h"
#include "TestUtils.h"
#include "Mdt/DeployUtils/Impl/Elf/DynamicSection.h"

using namespace Mdt::DeployUtils;
using Mdt::DeployUtils::Impl::Elf::DynamicStruct;
using Mdt::DeployUtils::Impl::Elf::DynamicSection;
using Mdt::DeployUtils::Impl::Elf::StringTable;
using Mdt::DeployUtils::Impl::Elf::DynamicSectionTagType;

DynamicStruct makeEntry(DynamicSectionTagType tag, uint64_t valOrPtr)
{
  DynamicStruct entry(tag);
  entry.val_or_ptr = valOrPtr;

  return entry;
}

DynamicStruct makeNullEntry()
{
  return makeEntry(DynamicSectionTagType::Null, 0);
}

DynamicStruct makeNeededEntry(uint64_t val = 2)
{
  return makeEntry(DynamicSectionTagType::Needed, val);
}

DynamicStruct makeSoNameEntry(uint64_t val = 2)
{
  return makeEntry(DynamicSectionTagType::SoName, val);
}

DynamicStruct makeRunPathEntry(uint64_t val = 2)
{
  return makeEntry(DynamicSectionTagType::Runpath, val);
}

DynamicStruct makeStringTableAddressEntry(uint64_t address)
{
  return makeEntry(DynamicSectionTagType::StringTable, address);
}

DynamicStruct makeStringTableSizeEntry(uint64_t size)
{
  return makeEntry(DynamicSectionTagType::StringTableSize, size);
}

StringTable stringTableFromCharArray(unsigned char * const array, qint64 size)
{
  return StringTable::fromCharArray( arraySpanFromArray(array, size) );
}
