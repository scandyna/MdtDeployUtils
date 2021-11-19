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
#ifndef ELF_SECTION_HEADER_TEST_UTILS_H
#define ELF_SECTION_HEADER_TEST_UTILS_H

#include "Mdt/DeployUtils/Impl/Elf/SectionHeader.h"
#include <string>

using Mdt::DeployUtils::Impl::Elf::SectionHeader;

inline
SectionHeader makeNullSectionHeader()
{
  SectionHeader header;
  header.type = 0;
  header.offset = 0;
  header.addr = 0;
  header.size = 0;

  return header;
}

inline
SectionHeader makeDynamicSectionHeader()
{
  SectionHeader header;
  header.name = ".dynamic";
  header.type = 6;
  header.link = 0;

  return header;
}

inline
SectionHeader makeStringTableSectionHeader(const std::string & name = "")
{
  SectionHeader header;
  header.name = name;
  header.type = 3;

  return header;
}

inline
SectionHeader makeDynamicStringTableSectionHeader()
{
  return makeStringTableSectionHeader(".dynstr");
}

inline
SectionHeader makeDynamicLinkerSymbolTableSectionHeader()
{
  SectionHeader header;
  header.name = ".dynsym";
  header.type = 0xB;
  header.link = 0;

  return header;
}

inline
SectionHeader makeSymbolTableSectionHeader()
{
  SectionHeader header;
  header.name = ".symtab";
  header.type = 0x2;
  header.link = 0;

  return header;
}

inline
SectionHeader makeGlobalOffsetTableSectionHeader(const std::string & name)
{
  SectionHeader header;
  header.type = 1;
  header.name = name;
  header.offset = 0;
  header.size = 0;

  return header;
}

inline
SectionHeader makeGotSectionHeader()
{
  return makeGlobalOffsetTableSectionHeader(".got");
}

inline
SectionHeader makeGotPltSectionHeader()
{
  return makeGlobalOffsetTableSectionHeader(".got.plt");
}

inline
SectionHeader makeProgramInterpreterSectionHeader()
{
  SectionHeader header;
  header.type = 1;
  header.name = ".interp";
  header.offset = 0;
  header.size = 0;

  return header;
}

inline
SectionHeader makeGnuHashTableSectionHeader()
{
  SectionHeader header;
  header.type = 0x6ffffff6;
  header.name = ".gnu.hash";
  header.offset = 0;
  header.size = 0;

  return header;
}

inline
SectionHeader makeNoteSectionHeader(const std::string & name)
{
  SectionHeader header;
  header.type = 7;
  header.name = name;
  header.offset = 0;
  header.size = 0;

  return header;
}

inline
SectionHeader makeRelocationWithAddendSectionHeader(const std::string & name)
{
  SectionHeader header;
  header.type = 4;
  header.name = name;

  return header;
}

#endif // #ifndef ELF_SECTION_HEADER_TEST_UTILS_H
