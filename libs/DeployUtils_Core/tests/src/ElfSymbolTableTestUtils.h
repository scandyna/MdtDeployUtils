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
#include "Mdt/DeployUtils/Impl/Elf/SymbolTable.h"

using Mdt::DeployUtils::Impl::Elf::SymbolTableEntry;
using Mdt::DeployUtils::Impl::Elf::PartialSymbolTableEntry;
using Mdt::DeployUtils::Impl::Elf::PartialSymbolTableEntry;

SymbolTableEntry makeSectionAssociationSymbolTableEntry()
{
  SymbolTableEntry entry;
  entry.info = 3;

  return entry;
}

PartialSymbolTableEntry makeSectionAssociationSymbolTableEntryWithFileOffset(uint64_t offset)
{
  PartialSymbolTableEntry pEntry;
  pEntry.fileOffset = offset;
  pEntry.entry = makeSectionAssociationSymbolTableEntry();

  return pEntry;
}
