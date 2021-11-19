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
#ifndef ELF_PROGRAM_HEADER_TEST_UTILS_H
#define ELF_PROGRAM_HEADER_TEST_UTILS_H

#include "Mdt/DeployUtils/Impl/Elf/ProgramHeader.h"

inline
Mdt::DeployUtils::Impl::Elf::ProgramHeader makeNullProgramHeader()
{
  Mdt::DeployUtils::Impl::Elf::ProgramHeader header;
  header.type = 0;

  return header;
}

inline
Mdt::DeployUtils::Impl::Elf::ProgramHeader makeDynamicSectionProgramHeader()
{
  Mdt::DeployUtils::Impl::Elf::ProgramHeader header;
  header.type = 2;

  return header;
}

inline
Mdt::DeployUtils::Impl::Elf::ProgramHeader makeProgramInterpreterProgramHeader()
{
  using Mdt::DeployUtils::Impl::Elf::SegmentType;

  Mdt::DeployUtils::Impl::Elf::ProgramHeader header;
  header.setSegmentType(SegmentType::Interpreter);

  return header;
}

inline
Mdt::DeployUtils::Impl::Elf::ProgramHeader makeNoteProgramHeader()
{
  using Mdt::DeployUtils::Impl::Elf::SegmentType;

  Mdt::DeployUtils::Impl::Elf::ProgramHeader header;
  header.setSegmentType(SegmentType::Note);

  return header;
}

inline
Mdt::DeployUtils::Impl::Elf::ProgramHeader makeProgramHeaderTableProgramHeader()
{
  using Mdt::DeployUtils::Impl::Elf::SegmentType;

  Mdt::DeployUtils::Impl::Elf::ProgramHeader header;
  header.setSegmentType(SegmentType::ProgramHeaderTable);
  header.memsz = 0;
  header.filesz = 0;

  return header;
}

inline
Mdt::DeployUtils::Impl::Elf::ProgramHeader makeGnuRelRoProgramHeader()
{
  using Mdt::DeployUtils::Impl::Elf::SegmentType;

  Mdt::DeployUtils::Impl::Elf::ProgramHeader header;
  header.setSegmentType(SegmentType::GnuRelRo);

  return header;
}

#endif // #ifndef ELF_PROGRAM_HEADER_TEST_UTILS_H
