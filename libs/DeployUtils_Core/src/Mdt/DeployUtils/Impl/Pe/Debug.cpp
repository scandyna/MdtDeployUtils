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

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Pe{

QString qStringFromBool(bool b)
{
  if(b){
    return QLatin1String("true");
  }

  return QLatin1String("false");
}

QString toDebugString(const DosHeader & header)
{
  QString str = QLatin1String("PE signature offset: 0x") + QString::number(header.peSignatureOffset, 16);
  str += QLatin1String(" (") + QString::number(header.peSignatureOffset) + QLatin1String(")");

  return str;
}

QString toDebugString(MachineType type)
{
  switch(type){
    case MachineType::Unknown:
      return QLatin1String("Unknown (the content of this field is assumed to be applicable to any machine type)");
    case MachineType::Amd64:
      return QLatin1String("AMD64 (x64, x86-64)");
    case MachineType::I386:
      return QLatin1String("I386 (x86)");
    case MachineType::NotHandled:
      return QLatin1String("NotHandled");
    case MachineType::Null:
      return QLatin1String("Null");
  }

  return QString();
}

QString toDebugString(const CoffHeader & header)
{
  QString str = QLatin1String("Machine type: 0x") + QString::number(header.machine, 16)
                + QLatin1String(" (") + toDebugString( header.machineType() ) + QLatin1String(")");

  str += QLatin1String("\nsize of optional header: ") + QString::number(header.sizeOfOptionalHeader)
       + QLatin1String(" (0x") + QString::number(header.sizeOfOptionalHeader, 16) + QLatin1String(")");

  str += QLatin1String("\ncharacteristics: 0x") + QString::number(header.characteristics, 16);
  str += QLatin1String("\n is valid executable image (IMAGE_FILE_EXECUTABLE_IMAGE): ") + qStringFromBool( header.isValidExecutableImage() );
  str += QLatin1String("\n is 32-bit word architecture (IMAGE_FILE_32BIT_MACHINE): ") + qStringFromBool( header.is32BitWordMachine() );
  str += QLatin1String("\n is valid a DLL (IMAGE_FILE_DLL): ") + qStringFromBool( header.isDll() );

  return str;
}

QString toDebugString(MagicType type)
{
  switch(type){
    case MagicType::Unknown:
      return QLatin1String("Unknown");
    case MagicType::Pe32:
      return QLatin1String("PE32, 32-bit executable");
    case MagicType::Pe32Plus:
      return QLatin1String("PE32+, 64-bit executable");
    case MagicType::RomImage:
      return QLatin1String("ROM image");
  }

  return QString();
}

QString toDebugString(const OptionalHeader & header)
{
  QString str = QLatin1String("magic: ") + toDebugString( header.magicType() );

  str += QLatin1String("\nnumber of RVA and sizes: ") + QString::number(header.numberOfRvaAndSizes);
  if( header.containsImportTable() ){
    const ImageDataDirectory directory = header.importTableDirectory();
    str += QLatin1String("\ncontains the import table: address: 0x") + QString::number(directory.virtualAddress, 16)
        +  QLatin1String(", size: ") + QString::number(directory.size);
  }
  if( header.containsDelayImportTable() ){
    const ImageDataDirectory directory = header.delayImportTableDirectory();
    str += QLatin1String("\ncontains the delay import table: address: 0x") + QString::number(directory.virtualAddress, 16)
        +  QLatin1String(", size: ") + QString::number(directory.size);
  }else{
    str += QLatin1String("\ndoes not contain a delay import table");
  }

  return str;
}

QString toDebugString(const SectionHeader & header)
{
  QString str = QLatin1String("section header ") + header.name;
  str += QLatin1String("\n virtual size: ") + QString::number(header.virtualSize);
  str += QLatin1String("\n virtual address: 0x") + QString::number(header.virtualAddress, 16);
  str += QLatin1String("\n file pointer to raw data: 0x") + QString::number(header.pointerToRawData, 16);
  str += QLatin1String("\n size: ") + QString::number(header.sizeOfRawData);

  return str;
}

QString toDebugString(const ImportDirectory & directory, const QString & leftPad)
{
  QString str = leftPad + QLatin1String("DLL name RVA: 0x") + QString::number(directory.nameRVA, 16);

  return str;
}

QString toDebugString(const ImportDirectoryTable & directoryTable)
{
  QString str = QLatin1String("import directory table:");

  for(const auto & directory : directoryTable){
    str += QLatin1String("\n") + toDebugString(directory, QLatin1String("  "));
  }

  return str;
}

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Pe{
