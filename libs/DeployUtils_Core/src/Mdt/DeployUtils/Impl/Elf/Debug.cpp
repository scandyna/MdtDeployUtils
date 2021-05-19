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
#include "Debug.h"
#include <QLatin1String>
#include <QLatin1Char>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

QString toDebugString(Class c)
{
  switch(c){
    case Class::ClassNone:
      return QLatin1String("ClassNone");
    case Class::Class32:
      return QLatin1String("Class32");
    case Class::Class64:
      return QLatin1String("Class64");
  }

  return QString();
}

QString toDebugString(DataFormat dataFormat)
{
  switch(dataFormat){
    case DataFormat::Data2LSB:
      return QLatin1String("Two's complement, little-endian");
    case DataFormat::Data2MSB:
      return QLatin1String("Two's complement, big-endian");
    case DataFormat::DataNone:
      return QLatin1String("unknown");
  }

  return QString();
}

QString toDebugString(OsAbiType osAbiType)
{
  switch(osAbiType){
    case OsAbiType::SystemV:
      return QLatin1String("System V");
    case OsAbiType::Linux:
      return QLatin1String("Linux");
    case OsAbiType::Unknown:
      return QLatin1String("unknown");
    case OsAbiType::ArchitectureSpecific:
      return QLatin1String("architecture specific");
  }

  return QString();
}

QString toDebugString(const Ident & ident)
{
  QString str;

  str = QLatin1String("Has valid magic number: ");
  if(ident.hasValidElfMagicNumber){
    str += QLatin1String("yes\n");
  }else{
    str += QLatin1String("no\n");
  }

  str += QLatin1String("class: ") + toDebugString(ident._class);
  str += QLatin1String("\nData format: ") + toDebugString(ident.dataFormat);
  str += QLatin1String("\nELF version: ") + QString::number(ident.version);
  str += QLatin1String("\nOS ABI: ") + QString::number(ident.osabi)
         + QLatin1String(" (") + toDebugString( ident.osAbiType() ) + QLatin1String(")");
  str += QLatin1String("\nABI version: ") + QString::number(ident.abiversion);

  return str;
}

QString toDebugString(ObjectFileType type)
{
  switch(type){
    case ObjectFileType::None:
      return QLatin1String("None");
    case ObjectFileType::RelocatableFile:
      return QLatin1String("REL (relocatable file)");
    case ObjectFileType::ExecutableFile:
      return QLatin1String("EXEC (executable file)");
    case ObjectFileType::SharedObject:
      return QLatin1String("DYN (shared object)");
    case ObjectFileType::CoreFile:
      return QLatin1String("CORE (core file)");
    case ObjectFileType::Unknown:
      return QLatin1String("unknown");
  }

  return QString();
}

QString toDebugString(Machine machine)
{
  switch(machine){
    case Machine::None:
      return QLatin1String("None");
    case Machine::X86:
      return QLatin1String("x86");
    case Machine::X86_64:
      return QLatin1String("AMD x86-64");
    case Machine::Unknown:
      return QLatin1String("Unknown");
  }

  return QString();
}

QString toDebugString(const FileHeader & header)
{
  QString str = toDebugString(header.ident);

  str += QLatin1String("\nObject file type: ") + toDebugString(header.type);
  str += QLatin1String("\nMachine: ") + toDebugString(header.machine);
  str += QLatin1String("\nVersion: ") + QString::number(header.version);
  str += QLatin1String("\nEntry point: 0x") + QString::number(header.entry, 16);
  str += QLatin1String("\nProgram header offset: ") + QString::number(header.phoff);

  return str;
}

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{
