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
#include <QStringList>
#include <QObject>
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
    assert( dosHeader.seemsValid() );

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
   */
  inline
  bool rvaIsInSection(uint32_t rva, const SectionHeader & sectionHeader) noexcept
  {
    assert( sectionHeader.seemsValid() );

    const uint32_t start = sectionHeader.virtualAddress;
    const uint32_t end = start + sectionHeader.virtualSize;

    return (rva >= start) && (rva < end);
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

    int64_t offset = sectionTableOffset(coffHeader, dosHeader);
    for(uint16_t i = 1; i < coffHeader.numberOfSections; ++i){
      const SectionHeader sectionHeader = sectionHeaderFromArray( map.subSpan(offset, 40) );
      if( sectionHeader.seemsValid() && rvaIsInSection(directory.virtualAddress, sectionHeader) ){
        return sectionHeader;
      }
      offset += 40;
    }

    return SectionHeader();
  }

  /*! \internal
   */
  inline int64_t minimumSizeToExtractSection(const SectionHeader & sectionHeader, const ImageDataDirectory & directory) noexcept
  {
    assert( sectionHeader.seemsValid() );
    assert( !directory.isNull() );
    assert( sectionHeader.rvaIsValid(directory.virtualAddress) );

    return sectionHeader.rvaToFileOffset(directory.virtualAddress) + directory.size;
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
   */
  inline
  ImportDirectoryTable importDirectoryTableFromArray(const ByteArraySpan & map) noexcept
  {
    assert( !map.isNull() );
    assert( map.size >= 20 );

    ImportDirectoryTable table;

    /*
     * We must not extract the whole section,
     * but only the import directory table.
     * This table ends with a null directory.
     *
     * see https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#the-idata-section
     */
    const int64_t lastOffset = map.size - 20;
    for( int64_t offset = 0; offset <= lastOffset; offset += 20 ){
      const ImportDirectory directory = importDirectoryFromArray( map.subSpan(offset, 20) );
      if( directory.isNull() ){
        return table;
      }
      table.push_back(directory);
    }

    return table;
  }

  /*! \internal
   */
  inline
  ImportDirectoryTable extractImportDirectoryTable(const ByteArraySpan & map, const SectionHeader & sectionHeader, const ImageDataDirectory & directory) noexcept
  {
    assert( !map.isNull() );
    assert( sectionHeader.seemsValid() );
    assert( !directory.isNull() );
    assert( sectionHeader.rvaIsValid(directory.virtualAddress) );
    assert( map.size >= minimumSizeToExtractSection(sectionHeader, directory) );

    const int64_t offset = sectionHeader.rvaToFileOffset(directory.virtualAddress);
    const int64_t size = directory.size;

    return importDirectoryTableFromArray( map.subSpan(offset, size) );
  }

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
    assert( map.size >= 32 );

    DelayLoadTable table;

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
   */
  inline
  DelayLoadTable extractDelayLoadTable(const ByteArraySpan & map, const SectionHeader & sectionHeader, const ImageDataDirectory & directory) noexcept
  {
    assert( !map.isNull() );
    assert( sectionHeader.seemsValid() );
    assert( !directory.isNull() );
    assert( sectionHeader.rvaIsValid(directory.virtualAddress) );
    assert( map.size >= minimumSizeToExtractSection(sectionHeader, directory) );

    const int64_t offset = sectionHeader.rvaToFileOffset(directory.virtualAddress);
    const int64_t size = directory.size;

    return delayLoadTableFromArray( map.subSpan(offset, size) );
  }

  /*! \internal
   *
   * \exception NotNullTerminatedStringError
   */
  inline
  QString extractDllName(const ByteArraySpan & map, const SectionHeader & sectionHeader, const ImportDirectory & directory)
  {
    assert( !map.isNull() );
    assert( sectionHeader.seemsValid() );
    assert( !directory.isNull() );
    assert( sectionHeader.rvaIsValid(directory.nameRVA) );

    /// \todo map size
    /// \todo address/offset validity

    const int64_t offset = sectionHeader.rvaToFileOffset(directory.nameRVA);

    return qStringFromUft8ByteArraySpan( map.subSpan(offset, 200) );
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
    assert( sectionHeader.rvaIsValid(directory.nameRVA) );

    /// \todo map size
    /// \todo address/offset validity

    const int64_t offset = sectionHeader.rvaToFileOffset(directory.nameRVA);

    return qStringFromUft8ByteArraySpan( map.subSpan(offset, 200) );
  }

  /*! \internal
   */
  class FileReader : public QObject
  {
    Q_OBJECT

   public:

    DosHeader mDosHeader;
    CoffHeader mCoffHeader;
    OptionalHeader mOptionalHeader;

    /*! \brief Set the file name
     */
    void setFileName(const QString & name) noexcept
    {
      mFileName = name;
    }

    /*! \brief Clear
     */
    void clear() noexcept
    {
      mFileName.clear();
      mDosHeader.clear();
      mCoffHeader.clear();
      mOptionalHeader.clear();
    }

    QStringList getNeededSharedLibraries(const ByteArraySpan & map)
    {
      assert( !map.isNull() );

      QStringList dlls;

      extractDosHeaderIfNull(map);
      extractCoffHeaderIfNull(map);
      extractOptionalHeaderIfNull(map);

      if( mOptionalHeader.containsImportTable() ){
        const ImageDataDirectory directoryDescriptor = mOptionalHeader.importTableDirectory();
        const SectionHeader sectionHeader = findSectionHeader(map, directoryDescriptor, mCoffHeader, mDosHeader);
        if( !sectionHeader.seemsValid() ){
          const QString message = tr("file '%1' declares to have a import table, but related section could not be found")
                                  .arg(mFileName);
          throw ExecutableFileReadError(message);
        }

        if( !sectionHeader.rvaIsValid(directoryDescriptor.virtualAddress) ){
          const QString message = tr("file '%1' the import directory descriptor contains a invalid address to its section")
                                  .arg(mFileName);
          throw ExecutableFileReadError(message);
        }
        const ImportDirectoryTable importTable = extractImportDirectoryTable(map, sectionHeader, directoryDescriptor);
        for(const auto & directory : importTable){
          if( !sectionHeader.rvaIsValid(directory.nameRVA) ){
            const QString message = tr("file '%1' the import directory contains a invalid address to a DLL name")
                                    .arg(mFileName);
            throw ExecutableFileReadError(message);
          }
          try{
            dlls.push_back( extractDllName(map, sectionHeader, directory) );
          }catch(const NotNullTerminatedStringError & error){
            const QString message = tr("file '%1' failed to extract a DLL name from the import directory (no end of string found)")
                                    .arg(mFileName);
            throw ExecutableFileReadError(message);
          }
        }
      }

      if( mOptionalHeader.containsDelayImportTable() ){
        const ImageDataDirectory directoryDescriptor = mOptionalHeader.delayImportTableDirectory();
        const SectionHeader sectionHeader = findSectionHeader(map, directoryDescriptor, mCoffHeader, mDosHeader);
        if( !sectionHeader.seemsValid() ){
          const QString message = tr("file '%1' declares to have delay load table, but related section could not be found")
                                  .arg(mFileName);
          throw ExecutableFileReadError(message);
        }

        if( !sectionHeader.rvaIsValid(directoryDescriptor.virtualAddress) ){
          const QString message = tr("file '%1' the delay load directory descriptor contains a invalid address to its section")
                                  .arg(mFileName);
          throw ExecutableFileReadError(message);
        }
        const DelayLoadTable delayLoadTable = extractDelayLoadTable(map, sectionHeader, directoryDescriptor);
        for(const auto & directory : delayLoadTable){
          if( !sectionHeader.rvaIsValid(directory.nameRVA) ){
            const QString message = tr("file '%1' the delay load directory contains a invalid address to a DLL name")
                                    .arg(mFileName);
            throw ExecutableFileReadError(message);
          }
          try{
            dlls.push_back( extractDllName(map, sectionHeader, directory) );
          }catch(const NotNullTerminatedStringError & error){
            const QString message = tr("file '%1' failed to extract a DLL name from the delay load directory (no end of string found)")
                                    .arg(mFileName);
            throw ExecutableFileReadError(message);
          }
        }
      }

      return dlls;
    }

   private:

    void extractDosHeaderIfNull(const ByteArraySpan & map)
    {
      assert( !map.isNull() );

      if( map.size < 64 ){
        const QString message = tr("file '%1' is to small to be a PE file")
                                .arg(mFileName);
        throw ExecutableFileReadError(message);
      }

      if( mDosHeader.seemsValid() ){
        return ;
      }
      mDosHeader = extractDosHeader(map);
      if( !mDosHeader.seemsValid() ){
        const QString message = tr("file '%1' does not contain the DOS header (no access to PE signature)")
                                .arg(mFileName);
        throw ExecutableFileReadError(message);
      }
    }

    void extractCoffHeaderIfNull(const ByteArraySpan & map)
    {
      assert( !map.isNull() );
      assert( mDosHeader.seemsValid() );

      if( mCoffHeader.seemsValid() ){
        return;
      }

      if( map.size < minimumSizeToExtractCoffHeader(mDosHeader) ){
        const QString message = tr("file '%1' is to small to extract the COFF header")
                                .arg(mFileName);
        throw ExecutableFileReadError(message);
      }
      mCoffHeader = extractCoffHeader(map, mDosHeader);
      if( !mCoffHeader.seemsValid() ){
        const QString message = tr("file '%1' does not contain the COFF header")
                                .arg(mFileName);
        throw ExecutableFileReadError(message);
      }
    }

    void extractOptionalHeaderIfNull(const ByteArraySpan & map)
    {
      assert( !map.isNull() );
      assert( mDosHeader.seemsValid() );
      assert( mCoffHeader.seemsValid() );

      if( mOptionalHeader.seemsValid() ){
        return;
      }

      if( map.size < minimumSizeToExtractOptionalHeader(mCoffHeader, mDosHeader) ){
        const QString message = tr("file '%1' is to small to extract the Optional header")
                                .arg(mFileName);
        throw ExecutableFileReadError(message);
      }
      mOptionalHeader = extractOptionalHeader(map, mCoffHeader, mDosHeader);
      if( mOptionalHeader.seemsValid() ){
        const QString message = tr("file '%1' does not contain the Optional header")
                                .arg(mFileName);
        throw ExecutableFileReadError(message);
      }
    }

    QString mFileName;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Pe{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_PE_FILE_READER_H
