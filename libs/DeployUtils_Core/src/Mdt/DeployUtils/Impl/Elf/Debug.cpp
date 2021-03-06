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
  str += QLatin1String("\nProgram header offset: ") + QString::number(header.phoff) + QLatin1String(" (bytes in the file)");
  str += QLatin1String("\nSection header offset: ") + QString::number(header.shoff) + QLatin1String(" (bytes in the file)");
  str += QLatin1String("\nFlags: 0x") + QString::number(header.flags, 16);
  str += QLatin1String("\nFile header size: ") + QString::number(header.ehsize) + QLatin1String(" bytes");
  str += QLatin1String("\nProgram header table entry size: ") + QString::number(header.phentsize) + QLatin1String(" bytes");
  str += QLatin1String("\nProgram header table entries: ") + QString::number(header.phnum);
  str += QLatin1String("\nSection header table entry size: ") + QString::number(header.shentsize) + QLatin1String(" bytes");
  str += QLatin1String("\nSection header table entries: ") + QString::number(header.shnum);
  str += QLatin1String("\nIndex of section header table entry that contains section names: ") + QString::number(header.shstrndx);

  return str;
}

QString toDebugString(SectionType type)
{
  switch(type){
    case SectionType::Null:
      return QLatin1String("Null");
    case SectionType::ProgramData:
      return QLatin1String("program data");
    case SectionType::SymbolTable:
      return QLatin1String("symbol table");
    case SectionType::StringTable:
      return QLatin1String("string table");
    case SectionType::Rela:
      return QLatin1String("relocation entries with addends");
    case SectionType::Dynamic:
      return QLatin1String("dynamic linking information");
    case SectionType::Note:
      return QLatin1String("notes");
    case SectionType::NoBits:
      return QLatin1String("program space with no data (bss)");
    case SectionType::DynSym:
      return QLatin1String("dynamic linker symbol table");
    case SectionType::InitArray:
      return QLatin1String("array of constructors");
    case SectionType::FiniArray:
      return QLatin1String("array of destructors");
    case SectionType::OsSpecific:
      return QLatin1String("OS specific");
  }

  return QString();
}

QString toDebugString(const SectionHeader & header)
{
  QString str;

  str = QLatin1String("section ") + QString::fromStdString(header.name);
  str += QLatin1String("\n name index: ") + QString::number(header.nameIndex);
  str += QLatin1String("\n type: 0x") + QString::number(header.type, 16) + QLatin1String(" (") + toDebugString( header.sectionType() ) + QLatin1String(")");
  str += QLatin1String("\n offset in file: ") + QString::number(header.offset) + QLatin1String(" (0x") + QString::number(header.offset, 16) + QLatin1String(")");
  str += QLatin1String("\n size in the file: ") + QString::number(header.size);
  str += QLatin1String("\n link: ") + QString::number(header.link);

  return str;
}

QString toDebugString(const std::vector<SectionHeader> & headers)
{
  QString str;

  for(const auto & header : headers){
    str += QLatin1String("\n") + toDebugString(header);
  }

  return str;
}

QString toDebugString(DynamicSectionTagType type)
{
  switch(type){
    case DynamicSectionTagType::Null:
      return QLatin1String("end of the _DYNAMIC array");
    case DynamicSectionTagType::Needed:
      return QLatin1String("string table offset to get the needed library name");
    case DynamicSectionTagType::StringTable:
      return QLatin1String("address to the string table");
    case DynamicSectionTagType::StringTableSize:
      return QLatin1String("size of the string table (in bytes)");
    case DynamicSectionTagType::SoName:
      return QLatin1String("string table offset to get the shared object name");
    case DynamicSectionTagType::RPath:
      return QLatin1String("string table offset to get the search path");
    case DynamicSectionTagType::Runpath:
      return QLatin1String("string table offset to get the search path");
    case DynamicSectionTagType::Unknown:
      return QLatin1String("unknown");
  }

  return QString();
}

QString toDebugString(const DynamicStruct & section, const QString & leftPad)
{
  QString str;

  str += leftPad + QLatin1String("dynamic section, tag: ")
      + QString::number(section.tag) + QLatin1String(" (") + toDebugString( section.tagType() ) + QLatin1String(")")
      + QLatin1Char('\n') + leftPad + QLatin1String(" val or ptr: ") + QString::number(section.val_or_ptr);

  return str;
}

QString toDebugString(const std::vector<DynamicStruct> & sections, const QString & leftPad)
{
  QString str;

  for(const auto & section : sections){
    str += QLatin1String("\n") + toDebugString(section, leftPad);
  }

  return str;
}

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{
