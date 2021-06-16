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
#ifndef MDT_DEPLOY_UTILS_IMPL_PE_FILE_READER_H
#define MDT_DEPLOY_UTILS_IMPL_PE_FILE_READER_H

#include "FileHeader.h"
#include "SectionHeader.h"
#include "ImportDirectory.h"
#include "Exceptions.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include "Mdt/DeployUtils/Impl/ExecutableFileReaderUtils.h"
#include "Mdt/DeployUtils/ExecutableFileReadError.h"
#include <QCoreApplication>
#include <QString>
#include <string>
#include <cassert>

#include "Debug.h"
#include <iostream>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Pe{

  /*! \internal
   */
  inline
  QString tr(const char *sourceText) noexcept
  {
    assert( sourceText != nullptr );

    return QCoreApplication::translate("Mdt::DeployUtils::Impl::Pe::FileReader", sourceText);
  }

  /*! \internal
   *
   * \pre \a map must not be null
   * \pre \a map size must be >= 64 (0x3C + 4)
   */
  inline
  DosHeader extractDosHeader(const ByteArraySpan & map) noexcept
  {
    assert( !map.isNull() );
    assert( map.size >= 64 );

    DosHeader header;

    header.peSignatureOffset = get32BitValueLE( map.subSpan(0x3C, 4) );

    return header;
  }

  /*! \internal
   *
   * \pre \a header must be valid
   */
  inline
  int64_t minimumSizeToExtractPeSignature(const DosHeader & header) noexcept
  {
    assert( header.seemsValid() );

    return header.peSignatureOffset + 4;
  }

  /*! \internal
   *
   * \pre \a map must not be null
   * \pre \a dosHeader must be valid
   * \pre \a map size must be big enouth to extract the PE signature
   */
  inline
  bool containsPeSignature(const ByteArraySpan & map, const DosHeader & dosHeader) noexcept
  {
    assert( !map.isNull() );
    assert( dosHeader.seemsValid() );
    assert( map.size >= minimumSizeToExtractPeSignature(dosHeader) );

    return arraysAreEqual( map.data + dosHeader.peSignatureOffset, 4, {'P','E',0,0} );
  }

  /*! \internal
   *
   * \pre \a dosHeader must be valid
   */
  inline
  int64_t minimumSizeToExtractCoffHeader(const DosHeader & dosHeader) noexcept
  {
    assert( dosHeader.seemsValid() );

    return minimumSizeToExtractPeSignature(dosHeader) + 20;
  }

  /*! \internal
   *
   * \pre \a array must not be null
   * \pre \a array size must be 20
   */
  inline
  CoffHeader coffHeaderFromArray(const ByteArraySpan & array) noexcept
  {
    assert( !array.isNull() );
    assert( array.size == 20 );

    CoffHeader header;

    header.machine = get16BitValueLE(array);
    header.numberOfSections = get16BitValueLE( array.subSpan(2, 2) );
    header.timeDateStamp = get32BitValueLE( array.subSpan(4, 4) );
    header.pointerToSymbolTable = get32BitValueLE( array.subSpan(8, 4) );
    header.numberOfSymbols = get32BitValueLE( array.subSpan(12, 4) );
    header.sizeOfOptionalHeader = get16BitValueLE( array.subSpan(16, 2) );
    header.characteristics = get16BitValueLE( array.subSpan(18, 2) );

    return header;
  }

  /*! \internal
   *
   * \pre \a map must not be null
   * \pre \a dosHeader must be valid
   * \pre \a map must contain the PE signature
   * \pre \a map size must be big enouth to extract the COFF header
   */
  inline
  CoffHeader extractCoffHeader(const ByteArraySpan & map, const DosHeader & dosHeader) noexcept
  {
    assert( !map.isNull() );
    assert( dosHeader.seemsValid() );
    assert( containsPeSignature(map, dosHeader) );
    assert( map.size >= minimumSizeToExtractCoffHeader(dosHeader) );

    return coffHeaderFromArray( map.subSpan(dosHeader.peSignatureOffset + 4, 20) );
  }

  /*! \internal
   *
   * \pre \a coffHeader must be valid
   */
  inline
  int64_t minimumSizeToExtractOptionalHeader(const CoffHeader & coffHeader, const DosHeader & dosHeader) noexcept
  {
    assert( coffHeader.seemsValid() );

    return minimumSizeToExtractCoffHeader(dosHeader) + coffHeader.sizeOfOptionalHeader;
  }

  /*! \internal
   *
   * \pre \a dosHeader must be valid
   */
  inline
  int64_t optionalHeaderOffset(const DosHeader & dosHeader) noexcept
  {
    assert( dosHeader.seemsValid() );

    return dosHeader.peSignatureOffset + 24;
  }

  /*! \internal
   *
   * \pre \a map must not be null
   * \pre \a coffHeader must be valid
   * \pre \a map size must be coffHeader's sizeOfOptionalHeader
   * \exception InvalidMagicType
   */
  inline
  OptionalHeader optionalHeaderFromArray(const ByteArraySpan & map, const CoffHeader & coffHeader)
  {
    assert( !map.isNull() );
    assert( coffHeader.seemsValid() );
    assert( map.size == static_cast<int64_t>(coffHeader.sizeOfOptionalHeader) );

    OptionalHeader header;

    header.magic = get16BitValueLE(map);

    if( header.magicType() == MagicType::Pe32 ){
      header.numberOfRvaAndSizes = get32BitValueLE( map.subSpan(92, 4) );
      header.importTable = get64BitValueLE( map.subSpan(104, 8) );
      if( map.size >= 208 ){
        header.delayImportTable = get64BitValueLE( map.subSpan(200, 8) );
      }
    }else if( header.magicType() == MagicType::Pe32Plus ){
      header.numberOfRvaAndSizes = get32BitValueLE( map.subSpan(108, 4) );
      if( map.size >= 128 ){
        header.importTable = get64BitValueLE( map.subSpan(120, 8) );
      }
      if( map.size >= 224 ){
        header.delayImportTable = get64BitValueLE( map.subSpan(216, 8) );
      }
    }else{
      const QString message = tr("invalid or unsupported magic type: %1")
                              .arg( static_cast<int>(header.magic) );
      throw InvalidMagicType(message);
    }

    return header;
  }

  /*! \internal
   *
   * \pre \a map must not be null
   * \pre \a coffHeader must be valid
   * \pre \a dosHeader must be valid
   * \pre \a map size must be big enouth to extract the optional header
   * \exception InvalidMagicType
   */
  inline
  OptionalHeader extractOptionalHeader(const ByteArraySpan & map, const CoffHeader & coffHeader, const DosHeader & dosHeader)
  {
    assert( !map.isNull() );
    assert( coffHeader.seemsValid() );
    assert( dosHeader.seemsValid() );
    assert( map.size >= minimumSizeToExtractOptionalHeader(coffHeader, dosHeader) );

    return optionalHeaderFromArray(map.subSpan(optionalHeaderOffset(dosHeader), coffHeader.sizeOfOptionalHeader), coffHeader);
  }

  /*! \internal Get a string from a array of unsigned characters
   *
   * Will read from \a charArray until a null char is encountered,
   * or until the end of the array.
   * This means that this function accepts non null terminated strings.
   *
   * \pre \a charArray must not be null
   * \sa https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#section-table-section-headers
   */
  inline
  QString qStringFromUft8BoundedUnsignedCharArray(const ByteArraySpan & charArray) noexcept
  {
    assert( !charArray.isNull() );

    if( charArray.data[charArray.size-1] != 0 ){
      return QString::fromUtf8( reinterpret_cast<const char*>(charArray.data), static_cast<int>(charArray.size) );
    }

    return QString::fromUtf8( reinterpret_cast<const char*>(charArray.data) );
  }

  /*! \internal Get a string from a array of unsigned characters
   *
   * The PE specification seems not to say anything about unicode encoding.
   * To get the DLL names from import directories, PE tells that it is ASCII.
   * So, assume UTF-8 and hope..
   * (note: using platform specific encoding can be problematic
   *        for cross-compilation)
   *
   * \pre \a charArray must not be null
   * \exception NotNullTerminatedStringError
   */
  inline
  QString qStringFromUft8ByteArraySpan(const ByteArraySpan & charArray)
  {
    assert( !charArray.isNull() );

    return qStringFromUft8UnsignedCharArray(charArray);
  }

  /*! \internal
   *
   * \pre \a coffHeader must be valid
   * \pre \a dosHeader must be valid
   */
  inline
  int64_t sectionTableOffset(const CoffHeader & coffHeader, const DosHeader & dosHeader) noexcept
  {
    assert( coffHeader.seemsValid() );
    assert( dosHeader.seemsValid() );

    return optionalHeaderOffset(dosHeader) + coffHeader.sizeOfOptionalHeader;
  }

  /*! \internal
   *
   * \pre \a coffHeader must be valid
   */
  inline
  int64_t sectionTableSize(const CoffHeader & coffHeader) noexcept
  {
    assert( coffHeader.seemsValid() );

    return coffHeader.numberOfSections * 40;
  }

  /*! \internal
   *
   * \pre \a coffHeader must be valid
   * \pre \a dosHeader must be valid
   */
  inline
  int64_t minimumSizeToExtractSectionTable(const CoffHeader & coffHeader, const DosHeader & dosHeader) noexcept
  {
    assert( coffHeader.seemsValid() );
    assert( dosHeader.seemsValid() );

    return sectionTableOffset(coffHeader, dosHeader) + sectionTableSize(coffHeader);
  }

  /*! \internal
   *
   * \pre \a map must not be null
   * \pre \a map size must be the size of a section header
   */
  inline
  SectionHeader sectionHeaderFromArray(const ByteArraySpan & map) noexcept
  {
    assert( !map.isNull() );
    assert( map.size == 40 );

    SectionHeader header;

    header.name = qStringFromUft8BoundedUnsignedCharArray( map.subSpan(0, 8) );
    header.virtualSize = get32BitValueLE( map.subSpan(8, 4) );
    header.virtualAddress = get32BitValueLE( map.subSpan(12, 4) );
    header.sizeOfRawData = get32BitValueLE( map.subSpan(16, 4) );
    header.pointerToRawData = get32BitValueLE( map.subSpan(20, 4) );

    return header;
  }

  /*! \internal
   *
   * \pre \a map must not be null
   * \pre \a name must not be empty
   * \pre \a coffHeader must be valid
   * \pre \a dosHeader must be valid
   * \pre \a map size must be big enouth to extract the the section table
   */
  inline
  SectionHeader findSectionHeader(const ByteArraySpan & map, const QString & name, const CoffHeader & coffHeader, const DosHeader & dosHeader) noexcept
  {
    assert( !map.isNull() );
    assert( !name.isEmpty() );
    assert( coffHeader.seemsValid() );
    assert( dosHeader.seemsValid() );
    assert( map.size >= minimumSizeToExtractSectionTable(coffHeader, dosHeader) );

    int64_t offset = sectionTableOffset(coffHeader, dosHeader);
    for(uint16_t i = 1; i < coffHeader.numberOfSections; ++i){
      SectionHeader sectionHeader = sectionHeaderFromArray( map.subSpan(offset, 40) );
      if( sectionHeader.name == name ){
        return sectionHeader;
      }
      offset += 40;
    }

    return SectionHeader();
  }

  /*! \internal
   */
  inline
  bool rvaIsInSection(uint32_t rva, const SectionHeader & sectionHeader) noexcept
  {
    assert( sectionHeader.seemsValid() );

    const uint32_t start = sectionHeader.virtualAddress;
    const uint32_t end = start + sectionHeader.virtualSize;

    return (rva >= start) && (rva < end);
  }

  /*! \internal
   */
  inline
  int64_t rvaToFileOffset(uint32_t rva, const SectionHeader & sectionHeader) noexcept
  {
    assert( rva > 0 );
    assert( sectionHeader.seemsValid() );
    assert( rvaIsInSection(rva, sectionHeader) );

    return sectionHeader.rvaToFileOffset(rva);
  }

  /*! \internal Find a section header from a optional header data directory
   *
   * \pre \a map must not be null
   * \pre \a directory must not be null
   * \pre \a coffHeader must be valid
   * \pre \a dosHeader must be valid
   * \pre \a map size must be big enouth to extract the the section table
   */
  inline
  SectionHeader findSectionHeader(const ByteArraySpan & map, const ImageDataDirectory & directory, const CoffHeader & coffHeader, const DosHeader & dosHeader) noexcept
  {
    assert( !map.isNull() );
    assert( !directory.isNull() );
    assert( coffHeader.seemsValid() );
    assert( dosHeader.seemsValid() );
    assert( map.size >= minimumSizeToExtractSectionTable(coffHeader, dosHeader) );

    std::cout << "searching section header for address 0x" << QString::number(directory.virtualAddress, 16).toStdString() << std::endl;
    
    int64_t offset = sectionTableOffset(coffHeader, dosHeader);
    for(uint16_t i = 1; i < coffHeader.numberOfSections; ++i){
      const SectionHeader sectionHeader = sectionHeaderFromArray( map.subSpan(offset, 40) );
      
//       std::cout << " candidat section header, name: " << sectionHeader.name.toStdString()
//       << ", virtual address: 0x" << QString::number(sectionHeader.virtualAddress, 16).toStdString() << std::endl;
      
//       std::cout << " candidate section header: " << toDebugString(sectionHeader).toStdString() << std::endl;
      
      if( sectionHeader.seemsValid() && rvaIsInSection(directory.virtualAddress, sectionHeader) ){
        return sectionHeader;
      }
      
//       const uint32_t start = sectionHeader.virtualAddress;
//       const uint32_t end = start + sectionHeader.virtualSize;
//       const uint32_t rva = directory.virtualAddress;
//       
//       if( (rva >= start) && (rva <= end) ){
//         return sectionHeader;
//       }
      
      
//       if( sectionHeader.virtualAddress == directory.virtualAddress ){
//         return sectionHeader;
//       }
      offset += 40;
    }

    return SectionHeader();
  }

  /*! \internal
   *
   * \pre \a map must not be null
   * \pre \a map size must be 20
   */
  inline
  ImportDirectory importDirectoryFromArray(const ByteArraySpan & map) noexcept
  {
    assert( !map.isNull() );
    assert( map.size == 20 );

    ImportDirectory directory;

    directory.nameRVA = get32BitValueLE( map.subSpan(12, 4) );

    return directory;
  }

  /*! \internal
   *
   * \pre \a map must not be null
   * 
   */
  inline
  ImportDirectoryTable importDirectoryTableFromArray(const ByteArraySpan & map) noexcept
  {
    assert( !map.isNull() );

    ImportDirectoryTable table;

    /// \todo check map size first
    
    std::cout << "get import directory table ...\n";
    
    /*
     * We must not extract the whole section,
     * but only the import directory table.
     * This table ends with a null directory.
     *
     * see https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#the-idata-section
     */
    const int64_t lastOffset = map.size - 20;
    for( int64_t offset = 0; offset <= lastOffset; offset += 20 ){
      
//       std::cout << " search at offset " << offset;
      
      const ImportDirectory directory = importDirectoryFromArray( map.subSpan(offset, 20) );
      
//       std::cout << " , directory dll RVA: " << directory.nameRVA << std::endl;
      
      if( directory.isNull() ){
        return table;
      }
      table.push_back(directory);
    }

    return table;
  }

  /*! \internal
   *
   * \pre \a map must not be null
   * 
   * \pre \a sectionHeader must be the .idata section header \todo maybe not
   * 
   * \todo min size to read it
   */
  inline
  ImportDirectoryTable extractImportDirectoryTable(const ByteArraySpan & map, const SectionHeader & sectionHeader, const ImageDataDirectory & directory) noexcept
  {
    assert( !map.isNull() );
    /// \todo min size to read it
    assert( sectionHeader.seemsValid() );
//     assert( sectionHeader.name == QLatin1String(".idata") );
    /// \todo assert on dir if..
    
//     const int64_t offset = sectionHeader.pointerToRawData;
//     const int64_t size = sectionHeader.sizeOfRawData;
    
    const int64_t offset = rvaToFileOffset( directory.virtualAddress, sectionHeader );
    const int64_t size = directory.size;

    return importDirectoryTableFromArray( map.subSpan(offset, size) );

//     return importDirectoryTableFromArray( map.subSpan( sectionHeader.pointerToRawData, sectionHeader.sizeOfRawData ) );
  }

  /*! \internal
   *
   * \pre \a map must not be null
   * \pre \a sectionHeader must be the .idata section header
   * 
   * \todo min size to read it
   */
//   inline
//   ImportDirectoryTable extractImportDirectoryTable(const ByteArraySpan & map, const ImageDataDirectory & directory) noexcept
//   {
//     assert( !map.isNull() );
//     /// \todo min size to read it
// 
//     return importDirectoryTableFromArray( map.subSpan( directory.virtualAddress, directory.size ) );
//   }

  /*! \internal
   *
   * \pre \a map must not be null
   * \pre \a map size must be 32
   */
  inline
  DelayLoadDirectory delayLoadDirectoryFromArray(const ByteArraySpan & map) noexcept
  {
    assert( !map.isNull() );
    assert( map.size == 32 );

    DelayLoadDirectory directory;

    directory.nameRVA = get32BitValueLE( map.subSpan(4, 4) );

    return directory;
  }

  /*! \internal
   *
   * \pre \a map must not be null
   * 
   */
  inline
  DelayLoadTable delayLoadTableFromArray(const ByteArraySpan & map) noexcept
  {
    assert( !map.isNull() );

    DelayLoadTable table;

    /// \todo check map size first

    /*
     * We must not extract the whole section,
     * but only the delay load table.
     * This table ends with a null directory.
     *
     * see https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#the-idata-section
     */
    const int64_t lastOffset = map.size - 32;
    for( int64_t offset = 0; offset <= lastOffset; offset += 32 ){
      const DelayLoadDirectory directory = delayLoadDirectoryFromArray( map.subSpan(offset, 32) );
      if( directory.isNull() ){
        return table;
      }
      table.push_back(directory);
    }

    return table;
  }

  /*! \internal
   *
   * \pre \a map must not be null
   * 
   * \pre \a sectionHeader must be the  \todo what is it
   * 
   * \todo min size to read it
   */
  inline
  DelayLoadTable extractDelayLoadTable(const ByteArraySpan & map, const SectionHeader & sectionHeader, const ImageDataDirectory & directory) noexcept
  {
    assert( !map.isNull() );
    /// \todo min size to read it
    assert( sectionHeader.seemsValid() );
    assert( !directory.isNull() );

    const int64_t offset = rvaToFileOffset( directory.virtualAddress, sectionHeader );
    const int64_t size = directory.size;

    return delayLoadTableFromArray( map.subSpan(offset, size) );
  }

  /*! \internal
   *
   * \exception NotNullTerminatedStringError
   */
  inline
  QString extractDllName(const ByteArraySpan & map, const SectionHeader & sectionHeader, const DelayLoadDirectory & directory)
  {
    assert( !map.isNull() );
    assert( sectionHeader.seemsValid() );
    assert( !directory.isNull() );

    /// \todo map size
    /// \todo address/offset validity
    
    const int64_t offset = sectionHeader.rvaToFileOffset(directory.nameRVA);
    
    return qStringFromUft8ByteArraySpan( map.subSpan(offset, 200) );
  }

  static std::string hexNumberString(uint64_t n)
  {
    return "0x" + QString::number(n, 16).toStdString();
  }
  
  static
  void sandbox(const ByteArraySpan & map)
  {
    assert( !map.isNull() );

    const DosHeader dosHeader = extractDosHeader(map);
    std::cout << toDebugString(dosHeader).toStdString() << std::endl;

    std::cout << "Contains PE signature: " << containsPeSignature(map, dosHeader) << std::endl;

    const CoffHeader coffHeader = extractCoffHeader(map, dosHeader);
    std::cout << toDebugString(coffHeader).toStdString() << std::endl;

    const OptionalHeader optionalHeader = extractOptionalHeader(map, coffHeader, dosHeader);
    std::cout << toDebugString(optionalHeader).toStdString() << std::endl;

//     const SectionHeader idataSectionHeader = findSectionHeader(map, QLatin1String(".idata"), coffHeader, dosHeader);
    const SectionHeader idataSectionHeader = findSectionHeader(map, optionalHeader.importTableDirectory(), coffHeader, dosHeader);
    if( !idataSectionHeader.seemsValid() ){
      //std::cerr << "could not find the .idata section header" << std::endl;
      std::cerr << "could not find the import table section header" << std::endl;
      return;
    }
    std::cout << toDebugString(idataSectionHeader).toStdString() << std::endl;
    
    std::cout << "import section header pointerToRawData: " << hexNumberString(idataSectionHeader.pointerToRawData) << std::endl;
    std::cout << "import image data directory, virtualAddress: "
              << hexNumberString(optionalHeader.importTableDirectory().virtualAddress)
              << ", file offset: " << hexNumberString( rvaToFileOffset(optionalHeader.importTableDirectory().virtualAddress, idataSectionHeader) )
              << std::endl;
    
    
    const ImportDirectoryTable importTable = extractImportDirectoryTable(map, idataSectionHeader, optionalHeader.importTableDirectory());
//     std::cout << toDebugString(importTable).toStdString() << std::endl;
    
//     const ImportDirectoryTable importTable = extractImportDirectoryTable( map, optionalHeader.importTableDirectory() );
//     std::cout << toDebugString(importTable).toStdString() << std::endl;

    
    for(const auto & directory : importTable){
      std::cout << " dll name file offset: 0x" << QString::number( directory.dllStringFileOffset(idataSectionHeader) , 16).toStdString();
      std::cout << ", name: " << qStringFromUft8ByteArraySpan( map.subSpan( directory.dllStringFileOffset(idataSectionHeader), 200 ) ).toStdString()  << std::endl;
    }
    
    if( optionalHeader.containsDelayImportTable() ){
      const ImageDataDirectory directoryDescriptor = optionalHeader.delayImportTableDirectory();
      const SectionHeader delayLoadSectionHeader = findSectionHeader(map, directoryDescriptor, coffHeader, dosHeader);
      if( !delayLoadSectionHeader.seemsValid() ){
        std::cerr << "could not find the section header for the delay load table" << std::endl;
        return;
      }
      const DelayLoadTable delayLoadTable = extractDelayLoadTable(map, delayLoadSectionHeader, directoryDescriptor);
      
      for(const auto & directory : delayLoadTable){
        std::cout << extractDllName(map, delayLoadSectionHeader, directory).toStdString() << std::endl;
      }
      
    }
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Pe{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_PE_FILE_READER_H
