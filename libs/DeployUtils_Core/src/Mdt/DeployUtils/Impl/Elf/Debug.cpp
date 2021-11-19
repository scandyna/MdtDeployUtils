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
#include "StringTable.h"
#include "SectionSegmentUtils.h"
#include <QLatin1String>
#include <QLatin1Char>
#include <algorithm>

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

  str += QLatin1String("\nObject file type: ") + toDebugString( header.objectFileType() );
  str += QLatin1String("\nMachine: ") + toDebugString( header.machineType() );
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

QString toDebugString(SegmentType type)
{
  switch(type){
    case SegmentType::Null:
      return QLatin1String("Null: Unused program header table entry");
    case SegmentType::Load:
      return QLatin1String("PT_LOAD: Loadable segment");
    case SegmentType::Dynamic:
      return QLatin1String("PT_DYNAMIC: Dynamic linking information");
    case SegmentType::Interpreter:
      return QLatin1String("PT_INTERP: Interpreter information");
    case SegmentType::Note:
      return QLatin1String("PT_NOTE: Auxiliary information");
    case SegmentType::ProgramHeaderTable:
      return QLatin1String("PT_PHDR: Program header table");
    case SegmentType::Tls:
      return QLatin1String("PT_TLS: Thread-Local Storage template");
    case SegmentType::GnuEhFrame:
      return QLatin1String("PT_GNU_EH_FRAME");
    case SegmentType::GnuStack:
      return QLatin1String("PT_GNU_STACK");
    case SegmentType::GnuRelRo:
      return QLatin1String("PT_GNU_RELRO");

    case SegmentType::Unknown:
      return QLatin1String("Unknown");
  }

  return QString();
}

QString segmentTypeName(SegmentType type)
{
  switch(type){
    case SegmentType::Null:
      return QLatin1String("PT_NULL");
    case SegmentType::Load:
      return QLatin1String("PT_LOAD");
    case SegmentType::Dynamic:
      return QLatin1String("PT_DYNAMIC");
    case SegmentType::Interpreter:
      return QLatin1String("PT_INTERP");
    case SegmentType::Note:
      return QLatin1String("PT_NOTE");
    case SegmentType::ProgramHeaderTable:
      return QLatin1String("PT_PHDR");
    case SegmentType::Tls:
      return QLatin1String("PT_TLS");
    case SegmentType::GnuEhFrame:
      return QLatin1String("PT_GNU_EH_FRAME");
    case SegmentType::GnuStack:
      return QLatin1String("PT_GNU_STACK");
    case SegmentType::GnuRelRo:
      return QLatin1String("PT_GNU_RELRO");
    case SegmentType::Unknown:
      return QLatin1String("Unknown");
  }

  return QString();
}

QString toDebugString(const ProgramHeader & header)
{
  QString str;

  str = QLatin1String("header for segment");
  str += QLatin1String("\n type: 0x") + QString::number(header.type, 16) + QLatin1String(" (") + toDebugString( header.segmentType() ) + QLatin1String(")");
  str += QLatin1String("\n offset in file: ") + QString::number(header.offset) + QLatin1String(" (0x") + QString::number(header.offset, 16) + QLatin1String(")");
  str += QLatin1String("\n virtual address in memory: ") + QString::number(header.vaddr)
       + QLatin1String(" (0x") + QString::number(header.vaddr, 16) + QLatin1String(")");
  str += QLatin1String("\n physical address in memory: ") + QString::number(header.paddr)
       + QLatin1String(" (0x") + QString::number(header.paddr, 16) + QLatin1String(")");
  str += QLatin1String("\n size in file: ") + QString::number(header.filesz) + QLatin1String(" [bytes]");
  str += QLatin1String("\n size in memory: ") + QString::number(header.memsz) + QLatin1String(" [bytes]");
  str += QLatin1String("\n flags: 0x") + QString::number(header.flags, 16);
  str += QLatin1String("\n alignment in memory: ") + QString::number(header.align) + QLatin1String(" (0x") + QString::number(header.align, 16) + QLatin1String(")");

  return str;
}

QString toDebugString(const ProgramHeaderTable & headers)
{
  QString str;

  for(const auto & header : headers){
    str += QLatin1String("\n") + toDebugString(header);
  }

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
    case SectionType::Rel:
      return QLatin1String("relocation entries without addends");
    case SectionType::DynSym:
      return QLatin1String("dynamic linker symbol table");
    case SectionType::InitArray:
      return QLatin1String("array of constructors");
    case SectionType::FiniArray:
      return QLatin1String("array of destructors");
    case SectionType::OsSpecific:
      return QLatin1String("OS specific");
    case SectionType::GnuHash:
      return QLatin1String("GNU_HASH: GNU hash table");
    case SectionType::GnuVersionDef:
      return QLatin1String("SHT_GNU_verdef: symbol versions that are provided");
    case SectionType::GnuVersionNeed:
      return QLatin1String("SHT_GNU_verneed: symbol versions that are required");
    case SectionType::GnuVersionSym:
      return QLatin1String("SHT_GNU_versym: Symbol Version Table");
  }

  return QString();
}

QString toDebugString(const SectionHeader & header)
{
  QString str;

  str = QLatin1String("header for section ") + QString::fromStdString(header.name);
  str += QLatin1String("\n name index: ") + QString::number(header.nameIndex);
  str += QLatin1String("\n type: 0x") + QString::number(header.type, 16) + QLatin1String(" (") + toDebugString( header.sectionType() ) + QLatin1String(")");
  str += QLatin1String("\n flags: 0x") + QString::number(header.flags, 16);
  str += QLatin1String("\n address: ") + QString::number(header.addr) + QLatin1String(" (0x") + QString::number(header.addr, 16) + QLatin1String(")");
  str += QLatin1String("\n address alignment: ") + QString::number(header.addralign) + QLatin1String(" (0x") + QString::number(header.addralign, 16) + QLatin1String(")");
  str += QLatin1String("\n offset in file: ") + QString::number(header.offset) + QLatin1String(" (0x") + QString::number(header.offset, 16) + QLatin1String(")");
  str += QLatin1String("\n size in the file: ") + QString::number(header.size);

  str += QLatin1String("\n info: ") + QString::number(header.info);

  str += QLatin1String("\n link: ") + QString::number(header.link);
  if( header.sectionType() == SectionType::Dynamic ){
    str += QLatin1String(" (section header index of the string table used by entries in the section)");
  }

  return str;
}

QString toDebugString(const std::vector<SectionHeader> & headers)
{
  QString str;

  for(size_t i=0; i<headers.size(); ++i){
    str += QLatin1String("\n[") + QString::number(i) + QLatin1String("] ") + toDebugString( headers.at(i) );
  }
//   for(const auto & header : headers){
//     str += QLatin1String("\n[] ") + toDebugString(header);
//   }

  return str;
}

QString sectionSegmentMappingToDebugString(const ProgramHeaderTable & programHeaderTable, const std::vector<SectionHeader> & sectionHeaderTable)
{
  QString str = QLatin1String("Section to segment mapping:");

  for(size_t i = 0; i < programHeaderTable.headerCount(); ++i){
    const QString index = QString( QLatin1String("%1") ).arg(i, 2);
    const QString segmentName = QString( QLatin1String("%1") ).arg(segmentTypeName( programHeaderTable.headerAt(i).segmentType() ), 16);

    str += QLatin1Char('\n') + index + QLatin1String(" ") + segmentName;

    for(const auto & sectionHeader : sectionHeaderTable){
      if( sectionIsInSegmentStrict( sectionHeader, programHeaderTable.headerAt(i) ) ){
        str += QLatin1String(" ") + QString::fromStdString(sectionHeader.name);
      }
    }
  }

  return str;
}

QString toDebugString(const StringTable & table)
{
  QString str;

  for(char c : table){
    if(c == 0){
      str.append( QLatin1String("\\0") );
    }else{
      str.append( QLatin1Char(c) );
    }
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
    case DynamicSectionTagType::PltGot:
      return QLatin1String("DT_PLTGOT");
    case DynamicSectionTagType::Hash:
      return QLatin1String("DT_HASH");
    case DynamicSectionTagType::StringTable:
      return QLatin1String("address to the string table");
    case DynamicSectionTagType::SymbolTable:
      return QLatin1String("DT_SYMTAB: address of the symbol table");
    case DynamicSectionTagType::RelocationTable:
      return QLatin1String("DT_RELA: address of the relocation table");
    case DynamicSectionTagType::RelocationTableSize:
      return QLatin1String("DT_RELASZ: total size [bytes] of the relocation table");
    case DynamicSectionTagType::RelocationEntrySize:
      return QLatin1String("DT_RELAENT: size [bytes] of the relocation entry");
    case DynamicSectionTagType::StringTableSize:
      return QLatin1String("size of the string table (in bytes)");
    case DynamicSectionTagType::SymbolEntrySize:
      return QLatin1String("DT_SYMENT: size [bytes] of a symbol table entry");
    case DynamicSectionTagType::Init:
      return QLatin1String("DT_INIT: address of the initialization function");
    case DynamicSectionTagType::Fini:
      return QLatin1String("DT_FINI: address of the termination function");
    case DynamicSectionTagType::SoName:
      return QLatin1String("string table offset to get the shared object name");
    case DynamicSectionTagType::RPath:
      return QLatin1String("string table offset to get the search path");
    case DynamicSectionTagType::Symbolic:
      return QLatin1String("DT_SYMBOLIC");
    case DynamicSectionTagType::Debug:
      return QLatin1String("DT_DEBUG: used for debugging");
    case DynamicSectionTagType::Runpath:
      return QLatin1String("string table offset to get the search path");
    case DynamicSectionTagType::GnuHash:
      return QLatin1String("DT_GNU_HASH");
    case DynamicSectionTagType::Unknown:
      return QLatin1String("unknown");
  }

  return QString();
}

QString dynamicStructValToDebugString(const DynamicStruct & entry)
{
  return QLatin1String("val: ") + QString::number(entry.val_or_ptr);
}

QString dynamicStructPtrToDebugString(const DynamicStruct & entry)
{
  return QLatin1String("ptr: 0x") + QString::number(entry.val_or_ptr, 16) + QLatin1String(" (") + QString::number(entry.val_or_ptr) + QLatin1String(")");
}

QString dynamicStructValOrPtrToDebugString(const DynamicStruct & entry)
{
  switch( entry.tagType() ){
    case DynamicSectionTagType::Null:
    case DynamicSectionTagType::Unknown:
      break;
    case DynamicSectionTagType::Needed:
      return dynamicStructValToDebugString(entry);
    case DynamicSectionTagType::PltGot:
    case DynamicSectionTagType::Hash:
    case DynamicSectionTagType::StringTable:
    case DynamicSectionTagType::SymbolTable:
    case DynamicSectionTagType::RelocationTable:
      return dynamicStructPtrToDebugString(entry);
    case DynamicSectionTagType::SoName:
    case DynamicSectionTagType::RelocationTableSize:
    case DynamicSectionTagType::RelocationEntrySize:
    case DynamicSectionTagType::SymbolEntrySize:
      return dynamicStructValToDebugString(entry);
    case DynamicSectionTagType::Init:
    case DynamicSectionTagType::Fini:
      return dynamicStructPtrToDebugString(entry);
    case DynamicSectionTagType::Runpath:
    case DynamicSectionTagType::RPath:
    case DynamicSectionTagType::StringTableSize:
      return dynamicStructValToDebugString(entry);
    case DynamicSectionTagType::Symbolic:
      return QLatin1String("ignored");
    case DynamicSectionTagType::Debug:
      return dynamicStructPtrToDebugString(entry);
    case DynamicSectionTagType::GnuHash:
      return dynamicStructPtrToDebugString(entry);
  }

  return QLatin1String("val or ptr: 0x") + QString::number(entry.val_or_ptr, 16) + QLatin1String(" (") + QString::number(entry.val_or_ptr) + QLatin1String(")");
}

QString toDebugString(const DynamicStruct & entry, const QString & leftPad)
{
  QString str;

  str += leftPad + QLatin1String("tag: ")
      + QString::number(entry.tag) + QLatin1String(" (") + toDebugString( entry.tagType() ) + QLatin1String(")")
      + QLatin1Char('\n') + leftPad + QLatin1String(" ") + dynamicStructValOrPtrToDebugString(entry);

  return str;
}

QString toDebugString(const DynamicSection & section, const QString & leftPad)
{
  QString str = QLatin1String(".dynamic section:");

  for(const auto & entry : section){
    str += QLatin1String("\n") + toDebugString(entry, leftPad);
  }

  return str;
}

QString toDebugString(SymbolType type)
{
  switch(type){
    case SymbolType::NoType:
      return QLatin1String("No type");
    case SymbolType::Object:
      return QLatin1String("Object");
    case SymbolType::Function:
      return QLatin1String("Function");
    case SymbolType::Section:
      return QLatin1String("Section");
    case SymbolType::File:
      return QLatin1String("File");
    case SymbolType::LowProc:
      return QLatin1String("Low proc");
    case SymbolType::HighProc:
      return QLatin1String("Hi proc");

  }

  return QLatin1String("??");
}

QString toDebugString(const SymbolTableEntry & entry, const QString & leftPad)
{
  QString str = leftPad;

  str += QLatin1String("name index: ") + QString::number(entry.name)
       + QLatin1String(", value: ") + QString::number(entry.value) + QLatin1String(" (0x") + QString::number(entry.value, 16)
       + QLatin1String("), size: ") + QString::number(entry.size)
//        + QLatin1String(", info: ") + QString::number(entry.info)
       + QLatin1String(",  type: ") + toDebugString( entry.symbolType() )
//        + QLatin1String(", other: ") + QString::number(entry.other)
       + QLatin1String(", shndx: ") + QString::number(entry.shndx);

  return str;
}

QString toDebugString(const PartialSymbolTable & table, const QString & leftPad)
{
  QString str;

  for(size_t i=0; i < table.entriesCount(); ++i){
    str += QLatin1String("\n") + toDebugString(table.entryAt(i), leftPad);
  }

  return str;
}

QString toDebugString(const GlobalOffsetTableEntry & entry, const QString & leftPad)
{
  QString str = leftPad;

  str += QLatin1String("data: ") + QString::number(entry.data) + QLatin1String(" (0x") + QString::number(entry.data, 16) + QLatin1String(")");

  return str;
}

QString toDebugString(const GlobalOffsetTable & table, const QString & leftPad)
{
  QString str;

  if( table.containsDynamicSectionAddress() ){
    str += QLatin1String("\n dynamic section address: 0x") + QString::number(table.dynamicSectionAddress(), 16);
  }

  for(size_t i=0; i < table.entriesCount(); ++i){
    str += QLatin1String("\n") + toDebugString(table.entryAt(i), leftPad);
  }

  return str;
}

QString toDebugString(const ProgramInterpreterSection & section)
{
  QString str = QLatin1String("program interpreter: ");

  str += QString::fromStdString(section.path);

  return str;
}

QString toDebugString(const GnuHashTable & table)
{
  QString str = QLatin1String("GNU hash table:");

  str += QString::fromLatin1("\n nbuckets: %1 , symoffset: %2 , bloom_size: %3 , bloom_shift: %4")
         .arg( table.bucketCount() ).arg(table.symoffset).arg( table.bloomSize() ).arg(table.bloomShift);

  str += QLatin1String("\n bloom array:");
  for(auto entry : table.bloom){
    str += QString::fromLatin1("\n  0x%1").arg(entry, 0, 16);
  }

  str += QLatin1String("\n buckets array:");
  for(auto entry : table.buckets){
    str += QString::fromLatin1("\n  %1 (0x%2)").arg(entry).arg(entry, 0, 16);
  }

  str += QLatin1String("\n chain array:");
  for(auto entry : table.chain){
    str += QString::fromLatin1("\n  0x%1").arg(entry, 0, 16);
  }

  return str;
}

QString toDebugString(const NoteSection & section, const QString & leftPad)
{
  QString str;

  str += leftPad + QLatin1String("owner: ") + QString::fromStdString(section.name);
  str += leftPad + QLatin1String("\ntype: ") + QString::number(section.type) + QLatin1String(" (0x") + QString::number(section.type, 16) + QLatin1String(")");
  for(size_t i=0; i<section.description.size(); ++i){
    const uint32_t word = section.description[i];
    str += QLatin1Char('\n') + leftPad + QString::fromLatin1(" word %1: %2 (0x%3)").arg(i).arg(word).arg(word, 0, 16);
  }

  return str;
}

QString toDebugString(const NoteSectionTable & table, const QString & leftPad)
{
  QString str;

  for(size_t i=0; i < table.sectionCount(); ++i){
    str += QString::fromLatin1("\nsection: %1 \n").arg( QString::fromStdString(table.sectionNameAt(i)) )
         + toDebugString(table.sectionAt(i), leftPad);
  }

  return str;
}

void sortProgramHeadersByFileOffset(ProgramHeaderTable & programHeaders)
{
  const auto cmp = [](const ProgramHeader & a, const ProgramHeader & b){
    return a.offset < b.offset;
  };

  std::sort(programHeaders.begin(), programHeaders.end(), cmp);
}

QString fileLayoutToDebugString(const FileHeader & fileHeader, ProgramHeaderTable programHeaders, std::vector<SectionHeader> sectionHeaders)
{
  QString str = QLatin1String("from 0 to 0x") + QString::number(fileHeader.ehsize-1, 16) + QLatin1String(": file header");

  str += QLatin1String("\nfrom 0x") + QString::number(fileHeader.phoff, 16)
       + QLatin1String(" to 0x") + QString::number(fileHeader.minimumSizeToReadAllProgramHeaders()-1, 16)
       + QLatin1String(": program headers table");

  sortProgramHeadersByFileOffset(programHeaders);
  for(const auto & programHeader : programHeaders){
    uint64_t last;
    if(programHeader.filesz > 0){
      last = programHeader.offset + programHeader.filesz - 1;
    }else{
      last = programHeader.offset;
    }
    uint64_t lastVaddr;
    if(programHeader.memsz > 0){
      lastVaddr = programHeader.vaddr + programHeader.memsz - 1;
    }else{
      lastVaddr = programHeader.vaddr;
    }
    str += QLatin1String("\nfrom 0x") + QString::number(programHeader.offset, 16)
        + QLatin1String(" to 0x") + QString::number(last, 16)
        + QLatin1String(" (vaddr 0x") + QString::number(programHeader.vaddr, 16)
        + QLatin1String(" to 0x") + QString::number(lastVaddr, 16)
        + QLatin1String("): segment of type ") + toDebugString(programHeader.segmentType());
  }

  sortSectionHeadersByFileOffset(sectionHeaders);
  for(const auto & sectionHeader : sectionHeaders){
    uint64_t last;
    uint64_t lastAddr;
    if(sectionHeader.size > 0){
      last = sectionHeader.offset + sectionHeader.size - 1;
      lastAddr = sectionHeader.addr + sectionHeader.size - 1;
    }else{
      last = sectionHeader.offset;
      lastAddr = sectionHeader.addr;
    }
    str += QLatin1String("\nfrom 0x") + QString::number(sectionHeader.offset, 16)
        + QLatin1String(" to 0x") + QString::number(last, 16);
        if(sectionHeader.addr > 0){
          str += QLatin1String(" (addr 0x") + QString::number(sectionHeader.addr, 16)
               + QLatin1String(" to 0x") + QString::number(lastAddr, 16) + QLatin1Char(')');
        }
        str += QLatin1String(": section ") + QString::fromStdString(sectionHeader.name);
        str += QString::fromLatin1(" (size: %1)").arg(sectionHeader.size);
  }

  str += QLatin1String("\nfrom 0x") + QString::number(fileHeader.shoff, 16)
       + QLatin1String(" to 0x") + QString::number(fileHeader.minimumSizeToReadAllSectionHeaders()-1, 16)
       + QLatin1String(": section headers table");

  return str;
}

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{
