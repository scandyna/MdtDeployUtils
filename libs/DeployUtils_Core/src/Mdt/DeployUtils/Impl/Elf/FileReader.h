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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_READER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_FILE_READER_H

#include "Exceptions.h"
#include "FileHeader.h"
#include "FileHeaderReaderWriterCommon.h"
#include "SectionHeader.h"
#include "SectionHeaderReaderWriterCommon.h"
#include "StringTable.h"
#include "DynamicSection.h"
#include "Mdt/DeployUtils/ExecutableFileReadError.h"
#include "Mdt/DeployUtils/Algorithm.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include "Mdt/DeployUtils/Impl/ExecutableFileReaderUtils.h"
#include <QChar>
#include <QString>
#include <QStringList>
#include <QObject>
#include <QtEndian>
#include <QCoreApplication>
#include <initializer_list>
#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>

// #include "Debug.h"
// #include <iostream>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  inline
  QString tr(const char *sourceText) noexcept
  {
    assert( sourceText != nullptr );

    return QCoreApplication::translate("Mdt::DeployUtils::Impl::Elf::FileReader", sourceText);
  }

  /*! \internal
   *
   * \pre \a dataFormat must be valid
   * \pre \a s must not be a nullptr
   * \pre \a the array referenced by \a s must ba at least 2 bytes long
   */
  inline
  uint16_t getHalfWord(const unsigned char * const s, DataFormat dataFormat) noexcept
  {
    assert( dataFormat != DataFormat::DataNone );
    assert( s != nullptr );

    uint16_t value;

    if( dataFormat == DataFormat::Data2MSB ){
      value = qFromBigEndian<uint16_t>(s);
    }else{
      value = qFromLittleEndian<uint16_t>(s);
    }

    return value;
  }

  /*! \internal
   *
   * \pre \a dataFormat must be valid
   * \pre \a s must not be a nullptr
   * \pre \a the array referenced by \a s must ba at least 4 bytes long
   */
  inline
  uint32_t getWord(const unsigned char * const s, DataFormat dataFormat) noexcept
  {
    assert( dataFormat != DataFormat::DataNone );
    assert( s != nullptr );

    uint32_t value;

    if( dataFormat == DataFormat::Data2MSB ){
      value = qFromBigEndian<uint32_t>(s);
    }else{
      value = qFromLittleEndian<uint32_t>(s);
    }

    return value;
  }

  /*! \internal
   *
   * \pre \a array must not be null
   * \pre \a array size must be at least 4 bytes long
   * \pre \a dataFormat must be valid
   */
  inline
  uint32_t getWord(const ByteArraySpan & array, DataFormat dataFormat) noexcept
  {
    assert( !array.isNull() );
    assert( array.size >= 4 );
    assert( dataFormat != DataFormat::DataNone );

    return getWord(array.data, dataFormat);
  }

  /*! \internal Get a (unsigned) word out from \a array
   *
   * Depending on the machine (32-bit or 64-bit), defined in \a ident ,
   * the value will be decoded as a uint32_t
   * or a uint64_t.
   *
   * The endianness, also defined in \a ident ,
   * is also taken into account for the decoding.
   *
   * \pre \a array must not be a nullptr
   * \pre \a array must be of a size that can hold the value
   *  (at least 4 bytes for a 32-bit file, at least 8 bytes for a 64-bit file)
   * \pre \a ident must be valid
   *
   * \sa https://manpages.debian.org/stretch/manpages/elf.5.en.html
   */
  inline
  uint64_t getNWord(const unsigned char * const array, const Ident & ident) noexcept
  {
    assert( array != nullptr );
    assert( ident.isValid() );

    if( ident._class == Class::Class32 ){
      return getWord(array, ident.dataFormat);
    }

    assert( ident._class == Class::Class64 );

    /*
     * using qFromBigEndian<uint64_t>(s);
     * or qFromLittleEndian<uint64_t>(s);
     * does not work (linker error),
     * so use Qt defined quint64
     */
    if( ident.dataFormat == DataFormat::Data2MSB ){
      return qFromBigEndian<quint64>(array);
    }

    assert( ident.dataFormat == DataFormat::Data2LSB );

    return qFromLittleEndian<quint64>(array);
  }

  /*! \internal Get a (unsigned) word out from \a array
   *
   * Depending on the machine (32-bit or 64-bit), defined in \a ident ,
   * the value will be decoded as a uint32_t
   * or a uint64_t.
   *
   * The endianness, also defined in \a ident ,
   * is also taken into account for the decoding.
   *
   * \pre \a array must not be null
   * \pre \a array must be of a size that can hold the value
   *  (at least 4 bytes for a 32-bit file, at least 8 bytes for a 64-bit file)
   * \pre \a ident must be valid
   *
   * \sa https://manpages.debian.org/stretch/manpages/elf.5.en.html
   */
  inline
  uint64_t getNWord(const ByteArraySpan & array, const Ident & ident) noexcept
  {
    assert( !array.isNull() );
    assert( ident.isValid() );

    return getNWord(array.data, ident);
  }

  /*! \internal
   *
   * \pre \a s must not be a nullptr
   * \pre \a the array referenced by \a s must ba at least
   *         4 bytes long for 32-bit file, 8 bytes long 64-bit file
   * \pre \a ident must be valid
   */
  inline
  uint64_t getAddress(const unsigned char * const s, const Ident & ident) noexcept
  {
    assert( s != nullptr );
    assert( ident.isValid() );

    return getNWord(s, ident);
  }

  /*! \internal
   *
   * \pre \a s must not be a nullptr
   * \pre \a the array referenced by \a s must ba at least
   *         4 bytes long for 32-bit file, 8 bytes long 64-bit file
   * \pre \a ident must be valid
   */
  inline
  uint64_t getOffset(const unsigned char * const s, const Ident & ident) noexcept
  {
    assert( s != nullptr );
    assert( ident.isValid() );

    return getAddress(s, ident);
  }

  /*! \internal Get a signed word out from \a array
   *
   * Depending on the machine (32-bit or 64-bit), defined in \a ident ,
   * the value will be decoded as a Elf32_Sword (int32_t)
   * or a Elf64_Sxword (int64_t).
   *
   * The endianness, also defined in \a ident ,
   * is also taken into account for the decoding.
   *
   * \pre \a array must not be a nullptr
   * \pre \a array must be of a size that can hold the value
   *  (at least 4 bytes for a 32-bit file, at least 8 bytes for a 64-bit file)
   * \pre \a ident must be valid
   *
   * \sa https://manpages.debian.org/stretch/manpages/elf.5.en.html
   */
  inline
  int64_t getSignedNWord(const unsigned char * const array, const Ident & ident) noexcept
  {
    assert( array != nullptr );
    assert( ident.isValid() );

    if( ident._class == Class::Class32 ){
      if( ident.dataFormat == DataFormat::Data2MSB ){
        return qFromBigEndian<int32_t>(array);
      }
      assert( ident.dataFormat == DataFormat::Data2LSB );

      return qFromLittleEndian<int32_t>(array);
    }

    assert( ident._class == Class::Class64 );

    if( ident.dataFormat == DataFormat::Data2MSB ){
      return qFromBigEndian<qint64>(array);
    }

    assert( ident.dataFormat == DataFormat::Data2LSB );

    return qFromLittleEndian<qint64>(array);
  }

  /*! \internal Get the next position after a address have been read
   *
   * \pre \a it must not be a nullptr
   * \pre \a ident must be valid
   */
  inline
  const unsigned char * nextPositionAfterAddress(const unsigned char * const it, const Ident & ident) noexcept
  {
    assert( it != nullptr );
    assert( ident.isValid() );

    if( ident._class == Class::Class32 ){
      return it + 4;
    }
    assert( ident._class == Class::Class64 );

    return it + 8;
  }

  /*! \internal Advance \a it for 4 or 8 bytes depending on file class (32-bit or 64-bit file)
   */
  inline
  void advance4or8bytes(const unsigned char * & it, const Ident & ident) noexcept
  {
    assert( it != nullptr );
    assert( ident.isValid() );

    it = nextPositionAfterAddress(it, ident);
  }

  /*! \internal Check if the array referenced by \a start starts with the ELF magic number
   *
   * \pre \a start must not be a nullptr
   * \pre the array must be at least 4 bytes long
   */
  inline
  bool startsWithElfMagicNumber(const uchar * const start) noexcept
  {
    assert( start != nullptr );

    return arraysAreEqual(start, 4, {0x7F,'E','L','F'});
  }

  /*! \internal
   */
  inline
  Class identClassFromByte(uchar byte) noexcept
  {
    switch(byte){
      case 1:
        return Class::Class32;
      case 2:
        return Class::Class64;
    }

    return Class::ClassNone;
  }

  /*! \internal
   */
  inline
  DataFormat dataFormatFromByte(uchar byte) noexcept
  {
    switch(byte){
      case 1:
        return DataFormat::Data2LSB;
      case 2:
        return DataFormat::Data2MSB;
    }

    return DataFormat::DataNone;
  }

  /*! \internal
   *
   * \pre \a map must not be a nullptr
   * \pre the array referenced by \a map must have at least 16 bytes
   */
  inline
  Ident extractIdent(const ByteArraySpan & map) noexcept
  {
    assert( !map.isNull() );
    assert( map.size >= 16 );

    Ident ident;

    ident.hasValidElfMagicNumber = startsWithElfMagicNumber(map.data);
    ident._class = identClassFromByte( map.data[0x04] );
    ident.dataFormat = dataFormatFromByte( map.data[0x05] );
    ident.version = map.data[0x06];
    ident.osabi = map.data[0x07];
    ident.abiversion = map.data[0x08];

    return ident;
  }

  /*! \internal
   *
   * \pre \a valueArray must not be a nullptr
   * \pre the array referenced by \a valueArray must have at least 2 bytes
   * \pre \a dataFormat must be valid
   */
  inline
  uint16_t extract_e_type(const uchar * const valueArray, DataFormat dataFormat) noexcept
  {
    assert( valueArray != nullptr );
    assert( dataFormat != DataFormat::DataNone );

    return getHalfWord(valueArray, dataFormat);
  }

  /*! \internal
   *
   * \pre \a valueArray must not be a nullptr
   * \pre the array referenced by \a valueArray must have at least 2 bytes
   * \pre \a dataFormat must be valid
   */
  inline
  uint16_t extract_e_machine(const uchar * const valueArray, DataFormat dataFormat) noexcept
  {
    assert( valueArray != nullptr );
    assert( dataFormat != DataFormat::DataNone );

    return getHalfWord(valueArray, dataFormat);
  }


  /*! \internal
   *
   * \pre \a map must not be null
   * \pre the array referenced by \a map must have at least
   *       52 bytes for 32-bit file, 64 bytes for 64-bit file
   * \sa minimumSizeToReadFileHeader()
   */
  inline
  FileHeader extractFileHeader(const ByteArraySpan & map) noexcept
  {
    assert( !map.isNull() );

    FileHeader header;
    const unsigned char *it = map.data;

    header.ident = extractIdent(map);
    if( !header.ident.isValid() ){
      return header;
    }
    assert( map.size >= minimumSizeToReadFileHeader(header.ident) );
    it += 0x10;

    header.type = extract_e_type(it, header.ident.dataFormat);
    it += 2;

    header.machine = extract_e_machine(it, header.ident.dataFormat);
    it += 2;

    header.version = getWord(it, header.ident.dataFormat);
    it += 4;

    header.entry = getAddress(it, header.ident);
    it = nextPositionAfterAddress(it, header.ident);

    header.phoff = getOffset(it, header.ident);
    advance4or8bytes(it, header.ident);

    header.shoff = getOffset(it, header.ident);
    advance4or8bytes(it, header.ident);

    header.flags = getWord(it, header.ident.dataFormat);
    it += 4;

    header.ehsize = getHalfWord(it, header.ident.dataFormat);
    it += 2;

    header.phentsize = getHalfWord(it, header.ident.dataFormat);
    it += 2;

    header.phnum = getHalfWord(it, header.ident.dataFormat);
    it += 2;

    header.shentsize = getHalfWord(it, header.ident.dataFormat);
    it += 2;

    header.shnum = getHalfWord(it, header.ident.dataFormat);
    it += 2;

    header.shstrndx = getHalfWord(it, header.ident.dataFormat);

    return header;
  }

  /*! \internal Get a string from a array of unsigned characters
   *
   * Each character will be interpreded as a (signed) char,
   * meaning that values > 127 are invalid.
   *
   * If \a charArray could contain UTF-8,
   * use qStringFromUft8UnsignedCharArray()
   *
   * \todo see PE format, maybe this will go to some algorithm header later
   *
   * \pre \a charArray must not be null
   * \exception NotNullTerminatedStringError
   */
  inline
  std::string stringFromUnsignedCharArray(const ByteArraySpan & charArray)
  {
    assert( !charArray.isNull() );

    if( !containsEndOfString(charArray) ){
      const QString message = tr("failed to extract a string from a region (end of string not found)");
      throw NotNullTerminatedStringError(message);
    }

    return std::string( reinterpret_cast<const char*>(charArray.data) );
  }

  /*! \brief Check if \a header is a string table section header
   */
  inline
  bool headerIsStringTableSection(const SectionHeader & header) noexcept
  {
    return header.sectionType() == SectionType::StringTable;
  }

  /*! \internal Extract a string table from \a map
   *
   * Will return a copy of the string table in \a map
   * for the region defined by \a sectionHeader .
   *
   * \sa StringTable::fromCharArray()
   * \pre \a map must not be null
   * \pre \a sectionHeader must be the string table section header
   * \pre \a map must be big enough to extract the string table referenced by \a sectionHeader
   * \exception StringTableError
   */
  inline
  StringTable extractStringTable(const ByteArraySpan & map, const SectionHeader & sectionHeader)
  {
    assert( !map.isNull() );
    assert( headerIsStringTableSection(sectionHeader) );
    assert( map.size >= sectionHeader.minimumSizeToReadSection() );

    const int64_t offset = static_cast<int64_t>(sectionHeader.offset);
    const int64_t count = static_cast<int64_t>(sectionHeader.size);

    return StringTable::fromCharArray( map.subSpan(offset, count) );
  }

  /*! \internal
   *
   * \pre \a stringTableSectionHeader must be the string table section header
   * \exception NotNullTerminatedStringError
   */
  inline
  void setSectionHeaderName(const ByteArraySpan & map, const SectionHeader & stringTableSectionHeader,
                            SectionHeader & sectionHeader)
  {
    assert( !map.isNull() );
    assert( headerIsStringTableSection(stringTableSectionHeader) );

    const int64_t offset = static_cast<int64_t>(stringTableSectionHeader.offset + sectionHeader.nameIndex);
    const int64_t size = static_cast<int64_t>(stringTableSectionHeader.size - sectionHeader.nameIndex);
    const ByteArraySpan charArray = map.subSpan(offset, size);

    sectionHeader.name = stringFromUnsignedCharArray(charArray);
  }

  /*! \internal
   *
   * \pre \a stringTableSectionHeader must be the string table section header
   * \exception NotNullTerminatedStringError
   */
  inline
  void setSectionHeadersName(const ByteArraySpan & map, const SectionHeader & stringTableSectionHeader,
                             std::vector<SectionHeader> & sectionHeaders)
  {
    assert( !map.isNull() );
    assert( stringTableSectionHeader.sectionType() == SectionType::StringTable );

    for(auto & sectionHeader : sectionHeaders){
      setSectionHeaderName(map, stringTableSectionHeader, sectionHeader);
    }
  }

  /*! \internal
   *
   * \pre \a array must not be null
   * \pre the array referenced by \a array must have at least
   *     40 bytes for 32-bit files, 64 bytes for 64-bit files
   * \pre \a fileHeader must be valid
   * \note this function will not set the section header name
   * \sa setSectionHeaderName()
   */
  inline
  SectionHeader sectionHeaderFromArray(const ByteArraySpan & array, const FileHeader & fileHeader) noexcept
  {
    assert( !array.isNull() );
    assert( fileHeader.seemsValid() );
    assert( sectionHeaderArraySizeIsBigEnough(array, fileHeader) );

    SectionHeader sectionHeader;
    const Ident ident = fileHeader.ident;
    const unsigned char *it = array.data;

    sectionHeader.nameIndex = getWord(it, ident.dataFormat);
    it += 4;

    sectionHeader.type = getWord(it, ident.dataFormat);
    it += 4;

    sectionHeader.flags = getNWord(it, ident);
    advance4or8bytes(it, ident);

    sectionHeader.addr = getAddress(it, ident);
    advance4or8bytes(it, ident);

    sectionHeader.offset = getOffset(it, ident);
    advance4or8bytes(it, ident);

    sectionHeader.size = getOffset(it, ident);
    advance4or8bytes(it, ident);

    sectionHeader.link = getWord(it, ident.dataFormat);
    it += 4;

    sectionHeader.info = getWord(it, ident.dataFormat);
    it += 4;

    sectionHeader.addralign = getNWord(it, ident);
    advance4or8bytes(it, ident);

    sectionHeader.entsize = getNWord(it, ident);

    return sectionHeader;
  }

  /*! \internal
   *
   * \note this function will not set the section header name
   * \pre \a map must not be null
   * \pre \a index must be < file header's section headers count
   * \sa setSectionHeaderName()
   */
  inline
  SectionHeader extractSectionHeaderAt(const ByteArraySpan & map, const FileHeader & fileHeader, uint16_t index) noexcept
  {
    assert( !map.isNull() );
    assert( index < fileHeader.shnum );

    ByteArraySpan sectionArray;
    sectionArray.data = map.data + fileHeader.shoff + index * fileHeader.shentsize;
    sectionArray.size = fileHeader.shentsize;

    return sectionHeaderFromArray(sectionArray, fileHeader);
  }

  /*! \internal
   */
  inline
  SectionHeader extractSectionHeaderAt(const ByteArraySpan & map, const FileHeader & fileHeader, uint16_t index,
                                       const SectionHeader & sectionNamesStringTableSectionHeader) noexcept
  {
    assert( !map.isNull() );
    assert( index < fileHeader.shnum );
    assert( sectionNamesStringTableSectionHeader.sectionType() == SectionType::StringTable );

    SectionHeader sectionHeader = extractSectionHeaderAt(map, fileHeader, index);
    setSectionHeaderName(map, sectionNamesStringTableSectionHeader, sectionHeader);

    return sectionHeader;
  }

  /*! \internal
   *
   * \pre \a map must not be null
   */
  inline
  SectionHeader extractSectionNameStringTableHeader(const ByteArraySpan & map, const FileHeader & fileHeader) noexcept
  {
    assert( !map.isNull() );

    return extractSectionHeaderAt(map, fileHeader, fileHeader.shstrndx);
  }

  /*! \internal
   *
   * \pre \a map must not be null
   * \pre \a map must be big enough to read all section headers
   * \exception NotNullTerminatedStringError
   */
  inline
  std::vector<SectionHeader> extractAllSectionHeaders(const ByteArraySpan & map, const FileHeader & fileHeader)
  {
    assert( !map.isNull() );
    assert( fileHeader.seemsValid() );
    assert( map.size >= fileHeader.minimumSizeToReadAllSectionHeaders() );

    std::vector<SectionHeader> sectionHeaders;

    const uint16_t sectionHeaderCount = fileHeader.shnum;

    for(uint16_t i = 0; i < sectionHeaderCount; ++i){
      sectionHeaders.emplace_back( extractSectionHeaderAt(map, fileHeader, i) );
    }

    const SectionHeader stringTableSectionHeader = extractSectionNameStringTableHeader(map, fileHeader);
    setSectionHeadersName(map, stringTableSectionHeader, sectionHeaders);

    return sectionHeaders;
  }

  /*! \internal Find the index of the first section of a type and for which its name matches \a namePredicate
   *
   * If the requested section header does not exist,
   * 0 is returned (which the correspnds to a null section header)
   */
  template<typename UnaryPredicate>
  inline
  uint16_t findFirstSectionHeaderIndex(const ByteArraySpan & map, const FileHeader & fileHeader,
                                       const SectionHeader & sectionNamesStringTableSectionHeader,
                                       SectionType type, UnaryPredicate namePredicate)
  {
    assert( !map.isNull() );
    assert( fileHeader.seemsValid() );
    assert( map.size >= fileHeader.minimumSizeToReadAllSectionHeaders() );
    assert( sectionNamesStringTableSectionHeader.sectionType() == SectionType::StringTable );
    assert( type != SectionType::Null );

    SectionHeader sectionHeader;
    const uint16_t sectionHeaderCount = fileHeader.shnum;

    for(uint16_t i = 0; i < sectionHeaderCount; ++i){
      sectionHeader = extractSectionHeaderAt(map, fileHeader, i);
      if( sectionHeader.sectionType() == type ){
        setSectionHeaderName(map, sectionNamesStringTableSectionHeader, sectionHeader);
        if( namePredicate(sectionHeader.name) ){
          return i;
        }
      }
    }

    return 0;
  }

  /*! \internal Find the first section header for which its name matches \a namePredicate
   */
  template<typename UnaryPredicate>
  inline
  SectionHeader findFirstSectionHeader(const ByteArraySpan & map, const FileHeader & fileHeader,
                                       const SectionHeader & sectionNamesStringTableSectionHeader,
                                       SectionType type, UnaryPredicate namePredicate)
  {
    assert( !map.isNull() );
    assert( fileHeader.seemsValid() );
    assert( map.size >= fileHeader.minimumSizeToReadAllSectionHeaders() );
    assert( sectionNamesStringTableSectionHeader.sectionType() == SectionType::StringTable );
    assert( type != SectionType::Null );

    SectionHeader sectionHeader;
    const uint16_t sectionHeaderCount = fileHeader.shnum;

    for(uint16_t i = 0; i < sectionHeaderCount; ++i){
      sectionHeader = extractSectionHeaderAt(map, fileHeader, i);
      if( sectionHeader.sectionType() == type ){
        setSectionHeaderName(map, sectionNamesStringTableSectionHeader, sectionHeader);
        if( namePredicate(sectionHeader.name) ){
          return sectionHeader;
        }
      }
    }

    sectionHeader.type = 0;

    return sectionHeader;
  }

  /*! \internal Find the index of a section header by type and name
   *
   * If the requested section header does not exist,
   * 0 is returned (which the correspnds to a null section header)
   */
  inline
  uint16_t findSectionHeaderIndex(const ByteArraySpan & map, const FileHeader & fileHeader,
                                  const SectionHeader & sectionNamesStringTableSectionHeader,
                                  SectionType type, const std::string & name)
  {
    assert( !map.isNull() );
    assert( fileHeader.seemsValid() );
    assert( map.size >= fileHeader.minimumSizeToReadAllSectionHeaders() );
    assert( sectionNamesStringTableSectionHeader.sectionType() == SectionType::StringTable );
    assert( type != SectionType::Null );
    assert( !name.empty() );

    const auto namePredicate = [&name](const std::string & currentName){
      return currentName == name;
    };

    return findFirstSectionHeaderIndex(map, fileHeader, sectionNamesStringTableSectionHeader, type, namePredicate);
  }

  /*! \internal Find a section header by a type and a name
   *
   * If requested section header does not exist,
   * a Null section header is returned.
   *
   * \pre \a map must not be null
   * \pre \a fileHeader must be valid
   * \pre \a map must be big enough to read all section headers
   * \pre \a sectionNamesStringTableSectionHeader must be the section header names string table
   * \pre \a type must not be Null
   * \pre \a name must not be empty
   * \exception NotNullTerminatedStringError
   */
  inline
  SectionHeader findSectionHeader(const ByteArraySpan & map, const FileHeader & fileHeader,
                                  const SectionHeader & sectionNamesStringTableSectionHeader,
                                  SectionType type, const std::string & name)
  {
    assert( !map.isNull() );
    assert( fileHeader.seemsValid() );
    assert( map.size >= fileHeader.minimumSizeToReadAllSectionHeaders() );
    assert( sectionNamesStringTableSectionHeader.sectionType() == SectionType::StringTable );
    assert( type != SectionType::Null );
    assert( !name.empty() );

    const auto namePredicate = [&name](const std::string & currentName){
      return currentName == name;
    };

    return findFirstSectionHeader(map, fileHeader, sectionNamesStringTableSectionHeader, type, namePredicate);
  }


  /*! \brief Check if \a header is a dynamic section header
   */
  inline
  bool headerIsDynamicSection(const SectionHeader & header) noexcept
  {
    if( header.sectionType() != SectionType::Dynamic ){
      return false;
    }
    if( header.name != ".dynamic" ){
      return false;
    }

    return true;
  }

  /*! \internal Check if \a dynamicSectionHeader has a valid index to a dynamic string table
   *
   * \pre \a fileHeader must be valid
   * \pre \a dynamicSectionHeader must be a Dynamic section type and be named ".dynamic"
   */
  inline
  bool sectionHeaderHasValidIndexToDynamicStringTable(const FileHeader & fileHeader, const SectionHeader & dynamicSectionHeader) noexcept
  {
    assert( fileHeader.seemsValid() );
    assert( headerIsDynamicSection(dynamicSectionHeader) );

    const uint16_t index = static_cast<uint16_t>(dynamicSectionHeader.link);
    if( index == 0 ){
      // SHN_UNDEF
      return false;
    }
    if( index >= fileHeader.shnum ){
      return false;
    }

    return true;
  }

  /*! \internal Extract the dynamic section
   *
   * \pre \a map must not be null
   * \pre \a fileHeader must be valid
   * \pre \a map must be big enough to read all section headers
   * \pre \a sectionNamesStringTableSectionHeader must be the section header names string table
   * \exception DynamicSectionReadError
   * \exception StringTableError
   */
  inline
  DynamicSection extractDynamicSection(const ByteArraySpan & map, const FileHeader & fileHeader,
                                       const SectionHeader & sectionNamesStringTableSectionHeader)
  {
    assert( !map.isNull() );
    assert( fileHeader.seemsValid() );
    assert( map.size >= fileHeader.minimumSizeToReadAllSectionHeaders() );
    assert( sectionNamesStringTableSectionHeader.sectionType() == SectionType::StringTable );

    DynamicSection dynamicSection;

    const uint16_t dynamicSectionHeaderIndex = findSectionHeaderIndex(map, fileHeader, sectionNamesStringTableSectionHeader, SectionType::Dynamic, ".dynamic");
    if(dynamicSectionHeaderIndex == 0){
      return dynamicSection;
    }

    const SectionHeader dynamicSectionHeader  = extractSectionHeaderAt(map, fileHeader, dynamicSectionHeaderIndex, sectionNamesStringTableSectionHeader);
    assert( dynamicSectionHeader.sectionType() == SectionType::Dynamic );

    if( map.size < dynamicSectionHeader.minimumSizeToReadSection() ){
      const QString msg = tr(
        "file is to small to read the .dynamic section."
        " required size: %1 , file size: %2"
      ).arg( dynamicSectionHeader.minimumSizeToReadSection() ).arg(map.size);
      throw DynamicSectionReadError(msg);
    }

    if( !sectionHeaderHasValidIndexToDynamicStringTable(fileHeader, dynamicSectionHeader) ){
      const QString msg = tr("the dynamic section header contains a invalid index to the related string table.");
      throw DynamicSectionReadError(msg);
    }
    if( dynamicSectionHeader.link >= fileHeader.shnum ){
      const QString msg = tr(
        "the .dynamic section header's references a string table section header that does not exist."
        " referenced section header: %1 , number of section headers: %2"
      ).arg(dynamicSectionHeader.link).arg(fileHeader.shnum);
      throw DynamicSectionReadError(msg);
    }

    const uint16_t dynamicStringTableSectionHeaderIndex = static_cast<uint16_t>(dynamicSectionHeader.link);
    const SectionHeader dynamicStringTableSectionHeader = extractSectionHeaderAt(map, fileHeader, dynamicStringTableSectionHeaderIndex);
    if( !headerIsStringTableSection(dynamicStringTableSectionHeader) ){
      const QString msg = tr("the .dynamic section header's references a string table section header that is not a string table header.");
      throw DynamicSectionReadError(msg);
    }

    const unsigned char * first = map.data + dynamicSectionHeader.offset;
    const unsigned char * const last = first + dynamicSectionHeader.size;

    while(first < last){
      DynamicStruct entry;

      entry.tag = getSignedNWord(first, fileHeader.ident);
      advance4or8bytes(first, fileHeader.ident);

      entry.val_or_ptr = getNWord(first, fileHeader.ident);
      advance4or8bytes(first, fileHeader.ident);

      dynamicSection.addEntry(entry);
    }

    if( !dynamicSection.containsStringTableSizeEntry() ){
      const QString msg = tr("the .dynamic section does not contain the string table size entry (DT_STRSZ).");
      throw DynamicSectionReadError(msg);
    }

    StringTable dynamicStringTableSection = extractStringTable(map, dynamicStringTableSectionHeader);
    dynamicSection.setStringTable(dynamicStringTableSection);

    return dynamicSection;
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_READER_H
