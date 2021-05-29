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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_ELF_FILE_READER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_ELF_FILE_READER_H

#include "FileHeader.h"
#include "SectionHeader.h"
#include "Mdt/DeployUtils/ExecutableFileReadError.h"
#include <QFile>
#include <QFileInfo>
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

#include "Debug.h"
#include <QDebug>
#include <iostream>

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
   */
  struct ByteArraySpan
  {
    const unsigned char *data = nullptr;
    int64_t size = 0;

    bool isNull() const noexcept
    {
      return data == nullptr;
    }
  };

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
  int64_t getNWord(const unsigned char * const array, const Ident & ident) noexcept
  {
    assert( array != nullptr );
    assert( ident.isValid() );

    if( ident._class == Class::Class32 ){
      return getWord(array, ident.dataFormat);
    }

    assert( ident._class == Class::Class64 );

    /*
     * using qFromBigEndian<usint64_t>(s);
     * or qFromLittleEndian<usint64_t>(s);
     * does not work (linker error),
     * so use Qt defined quint64
     */
    if( ident.dataFormat == DataFormat::Data2MSB ){
      return qFromBigEndian<quint64>(array);
    }

    assert( ident.dataFormat == DataFormat::Data2LSB );

    return qFromLittleEndian<quint64>(array);
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

  /*! \internal
   *
   * \pre \a start must not be a nullptr
   * \pre \a count must be >= 0
   */
  inline
  bool arraysAreEqual(const uchar * const start, qint64 count, std::initializer_list<uchar> reference) noexcept
  {
    assert( start != nullptr );
    assert( count >= 0 );

    const auto first = start;
    const auto last = start + count;

    return std::equal( first, last, reference.begin(), reference.end() );
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
  Ident extractIdent(const uchar * const map) noexcept
  {
    assert( map != nullptr );

    Ident ident;

    ident.hasValidElfMagicNumber = startsWithElfMagicNumber(map);
    ident._class = identClassFromByte( map[0x04] );
    ident.dataFormat = dataFormatFromByte( map[0x05] );
    ident.version = map[0x06];
    ident.osabi = map[0x07];
    ident.abiversion = map[0x08];

    return ident;
  }

  /*! \internal
   */
  inline
  ObjectFileType e_type_fromValue(uint16_t value) noexcept
  {
    switch(value){
      case 0x00:
        return ObjectFileType::None;
      case 0x01:
        return ObjectFileType::RelocatableFile;
      case 0x02:
        return ObjectFileType::ExecutableFile;
      case 0x03:
        return ObjectFileType::SharedObject;
      case 0x04:
        return ObjectFileType::CoreFile;
    }

    return ObjectFileType::Unknown;
  }

  /*! \internal
   *
   * \pre \a valueArray must not be a nullptr
   * \pre the array referenced by \a valueArray must have at least 2 bytes
   * \pre \a dataFormat must be valid
   */
  inline
  ObjectFileType extract_e_type(const uchar * const valueArray, DataFormat dataFormat) noexcept
  {
    assert( valueArray != nullptr );
    assert( dataFormat != DataFormat::DataNone );

    const uint16_t value = getHalfWord(valueArray, dataFormat);

    return e_type_fromValue(value);
  }

  /*! \internal
   */
  inline
  Machine e_machine_fromValue(uint16_t value) noexcept
  {
    switch(value){
      case 0x00:
        return Machine::None;
      case 0x03:
        return Machine::X86;
      case 0x3E:
        return Machine::X86_64;
    }

    return Machine::Unknown;
  }

  /*! \internal
   *
   * \pre \a valueArray must not be a nullptr
   * \pre the array referenced by \a valueArray must have at least 2 bytes
   * \pre \a dataFormat must be valid
   */
  inline
  Machine extract_e_machine(const uchar * const valueArray, DataFormat dataFormat) noexcept
  {
    assert( valueArray != nullptr );
    assert( dataFormat != DataFormat::DataNone );

    const uint16_t value = getHalfWord(valueArray, dataFormat);

    return e_machine_fromValue(value);
  }

  /*! \internal
   *
   * \pre \a map must not be a nullptr
   * \pre the array referenced by \a map must have at least
   *       52 bytes for 32-bit file, 64 bytes for 64-bit file
   */
  inline
  FileHeader extractFileHeader(const uchar * const map) noexcept
  {
    assert( map != nullptr );

    FileHeader header;
    const unsigned char *it = map;

    header.ident = extractIdent(it);
    if( !header.ident.isValid() ){
      return header;
    }
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

  /*! \internal Check if \a charArray contains the end of string
   *
   * \pre \a charArray must not be null
   */
  inline
  bool containsEndOfString(const ByteArraySpan & charArray) noexcept
  {
    assert( !charArray.isNull() );

    const auto first = charArray.data;
    const auto last = charArray.data + charArray.size;

    return std::find(first, last, 0) != last;
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
   * \exception ExecutableFileReadError
   */
  inline
  std::string stringFromUnsignedCharArray(const ByteArraySpan & charArray)
  {
    assert( !charArray.isNull() );

    if( !containsEndOfString(charArray) ){
      const QString message = tr("failed to extract a string from a region (end of string not found)");
      throw ExecutableFileReadError(message);
    }

    return std::string( reinterpret_cast<const char*>(charArray.data) );
  }

  /*! \internal Get a string from a array of unsigned characters
   *
   * \todo see PE format, maybe this will go to some algorithm header later
   *
   * The ELF specification seems not to say anything about unicode encoding.
   * Because ELF is Unix centric, it is assumed that \a charArray
   * represents a UTF-8 string.
   * (note: using platform specific encoding can be problematic
   *        for cross-compilation)
   *
   * \pre \a charArray must not be null
   * \exception ExecutableFileReadError
   */
  inline
  QString qStringFromUft8UnsignedCharArray(const ByteArraySpan & charArray)
  {
    assert( !charArray.isNull() );

    if( !containsEndOfString(charArray) ){
      const QString message = tr("failed to extract a string from a region (end of string not found)");
      throw ExecutableFileReadError(message);
    }

    return QString::fromUtf8( reinterpret_cast<const char*>(charArray.data) );
  }

  /*! \internal
   *
   * \pre \a stringTableSectionHeader must be the string table section header
   * \exception ExecutableFileReadError
   */
  inline
  void setSectionHeaderName(const unsigned char * const map, const SectionHeader & stringTableSectionHeader,
                            SectionHeader & sectionHeader)
  {
    assert( stringTableSectionHeader.sectionType() == SectionType::StringTable );

    ByteArraySpan charArray;
    charArray.data = map + stringTableSectionHeader.offset + sectionHeader.nameIndex;
    charArray.size = stringTableSectionHeader.size - sectionHeader.nameIndex;

    sectionHeader.name = stringFromUnsignedCharArray(charArray);
  }

  /*! \internal
   *
   * \pre \a stringTableSectionHeader must be the string table section header
   * \exception ExecutableFileReadError
   */
  inline
  void setSectionHeadersName(const unsigned char * const map, const SectionHeader & stringTableSectionHeader,
                             std::vector<SectionHeader> & sectionHeaders)
  {
    assert( stringTableSectionHeader.sectionType() == SectionType::StringTable );

    for(auto & sectionHeader : sectionHeaders){
      setSectionHeaderName(map, stringTableSectionHeader, sectionHeader);
    }
  }

  /*! \internal
   *
   * \pre \a array must not be null
   * \pre \a ident must be valid
   * \sa sectionHeaderFromArray()
   */
  inline
  bool sectionHeaderArraySizeIsBigEnough(const ByteArraySpan & array, const Ident & ident) noexcept
  {
    assert( !array.isNull() );
    assert( ident.isValid() );

    if( ident._class == Class::Class32 ){
      return array.size >= 40;
    }
    assert( ident._class == Class::Class64 );

    return array.size >= 64;
  }

  /*! \internal
   *
   * \pre \a array must not be null
   * \pre the array referenced by \a array must have at least
   *     40 bytes for 32-bit files, 64 bytes for 64-bit files
   * \pre \a ident must be valid
   * \note this function will not set the section header name
   * \sa setSectionHeaderName()
   */
  inline
  SectionHeader sectionHeaderFromArray(const ByteArraySpan & array, const Ident & ident) noexcept
  {
    assert( !array.isNull() );
    assert( ident.isValid() );
    assert( sectionHeaderArraySizeIsBigEnough(array, ident) );

    SectionHeader sectionHeader;
    const unsigned char *it = array.data;

    sectionHeader.nameIndex = getWord(it, ident.dataFormat);
    it += 4;

    sectionHeader.type = getWord(it, ident.dataFormat);
    it += 4;

    // sh_flags
    advance4or8bytes(it, ident);

    // sh_addr
    advance4or8bytes(it, ident);

    sectionHeader.offset = getOffset(it, ident);
    advance4or8bytes(it, ident);

    sectionHeader.size = getOffset(it, ident);
    advance4or8bytes(it, ident);

    sectionHeader.link = getWord(it, ident.dataFormat);

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

    return sectionHeaderFromArray(sectionArray, fileHeader.ident);
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
   * \exception ExecutableFileReadError
   */
  inline
  std::vector<SectionHeader> extractAllSectionHeaders(const ByteArraySpan & map, const FileHeader & fileHeader)
  {
    assert( !map.isNull() );
    assert( map.size >= fileHeader.minimumSizeToReadAllSectionHeaders() );

    std::vector<SectionHeader> sectionHeaders;

    const uint16_t sectionHeaderCount = fileHeader.shnum;

    for(uint16_t i = 0; i < sectionHeaderCount; ++i){
      sectionHeaders.emplace_back( extractSectionHeaderAt(map, fileHeader, i) );
    }

    const SectionHeader stringTableSectionHeader = extractSectionNameStringTableHeader(map, fileHeader);
    setSectionHeadersName(map.data, stringTableSectionHeader, sectionHeaders);

    return sectionHeaders;
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
   * \exception ExecutableFileReadError
   */
  inline
  SectionHeader findSectionHeader(const ByteArraySpan & map, const FileHeader & fileHeader,
                                  const SectionHeader & sectionNamesStringTableSectionHeader,
                                  SectionType type, const std::string & name)
  {
    assert( !map.isNull() );
    ///assert( fileHeader.isValid() );
    assert( map.size >= fileHeader.minimumSizeToReadAllSectionHeaders() );
    assert( sectionNamesStringTableSectionHeader.sectionType() == SectionType::StringTable );
    assert( type != SectionType::Null );
    assert( !name.empty() );

    SectionHeader sectionHeader;
    const uint16_t sectionHeaderCount = fileHeader.shnum;

    for(uint16_t i = 0; i < sectionHeaderCount; ++i){
      sectionHeader = extractSectionHeaderAt(map, fileHeader, i);
      if( sectionHeader.sectionType() == type ){
        setSectionHeaderName(map.data, sectionNamesStringTableSectionHeader, sectionHeader);
        if( sectionHeader.name == name ){
          return sectionHeader;
        }
      }
    }

    sectionHeader.type = 0;

    return sectionHeader;
  }

  /*! \internal Extract a dynamic section for given tag
   *
   * \pre \a map must not be null
   * \pre \a ident must be valid
   * \pre \a dynamicSectionHeader must be a Dynamic section type and be named ".dynamic"
   * \pre \a map must be big enough to read the section referenced by \a dynamicSectionHeader
   */
  inline
  std::vector<DynamicStruct> extractDynamicSectionForTag(const ByteArraySpan & map, const Ident & ident,
                                                         const SectionHeader & dynamicSectionHeader,
                                                         DynamicSectionTagType tagType) noexcept
  {
    assert( !map.isNull() );
    assert( ident.isValid() );
    assert( dynamicSectionHeader.sectionType() == SectionType::Dynamic );
    assert( dynamicSectionHeader.name == ".dynamic" );
    assert( map.size >= dynamicSectionHeader.minimumSizeToReadSection() );

    std::vector<DynamicStruct> dynamicSection;

    const unsigned char * first = map.data + dynamicSectionHeader.offset;
    const unsigned char * const last = first + dynamicSectionHeader.size;

    while(first < last){
      DynamicStruct sectionStruct;

      sectionStruct.tag = getSignedNWord(first, ident);
      advance4or8bytes(first, ident);

      sectionStruct.val_or_ptr = getNWord(first, ident);
      advance4or8bytes(first, ident);

      if( sectionStruct.tagType() == tagType ){
        dynamicSection.push_back(sectionStruct);
      }
    }

    return dynamicSection;
  }

  /*! \internal Get a char array from \a dynamicStruct pointing to \a dynamicStringTableSectionHeader
   *
   * \pre \a map must not be null
   * \pre \a dynamicStringTableSectionHeader must be a string table header and be named ".dynstr"
   * \pre \a map must be big enough to read the section referenced by \a dynamicStringTableSectionHeader
   * \exception ExecutableFileReadError
   */
  inline
  ByteArraySpan charArrayFromDynamicStringTable(const ByteArraySpan & map,
                                                const SectionHeader & dynamicStringTableSectionHeader, const DynamicStruct & dynamicStruct)
  {
    assert( !map.isNull() );
    assert( dynamicStringTableSectionHeader.sectionType() == SectionType::StringTable );
    ///assert( dynamicStringTableSectionHeader.name == ".dynstr" );
    assert( map.size >= dynamicStringTableSectionHeader.minimumSizeToReadSection() );

    if( dynamicStruct.val_or_ptr > dynamicStringTableSectionHeader.size ){
      const QString message = tr("got a invalid index to a dynamic section string table");
      throw ExecutableFileReadError(message);
    }

    ByteArraySpan charArray;
    charArray.data = map.data + dynamicStringTableSectionHeader.offset + dynamicStruct.val_or_ptr;
    charArray.size = dynamicStringTableSectionHeader.size - dynamicStruct.val_or_ptr;

    return charArray;
  }

  /*! \internal Check if \a dynamicSectionHeader has a valid index to a dynamic string table
   *
   * \pre \a fileHeader must be valid
   * \pre \a dynamicSectionHeader must be a Dynamic section type and be named ".dynamic"
   */
  inline
  bool sectionHeaderHasValidIndexToDynamicStringTable(const FileHeader & fileHeader, const SectionHeader & dynamicSectionHeader) noexcept
  {
    assert( fileHeader.ident.isValid() );
    assert( dynamicSectionHeader.sectionType() == SectionType::Dynamic );
    assert( dynamicSectionHeader.name == ".dynamic" );

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

  /*! \internal Extract the needed shared libraries (DT_NEEDED)
   *
   * \pre \a map must not be null
   * \pre \a fileHeader must be valid
   * \pre \a dynamicSectionHeader must be a Dynamic section type and be named ".dynamic"
   * \pre \a dynamicSectionHeader must have a valid index to a dynamic string table
   * \pre \a map must be big enough to read the section referenced by \a dynamicSectionHeader
   * \exception ExecutableFileReadError
   */
  inline
  QStringList extractNeededSharedLibraries(const ByteArraySpan & map, const FileHeader & fileHeader, const SectionHeader & dynamicSectionHeader)
  {
    assert( !map.isNull() );
    assert( fileHeader.ident.isValid() );
    assert( dynamicSectionHeader.sectionType() == SectionType::Dynamic );
    assert( dynamicSectionHeader.name == ".dynamic" );
    assert( sectionHeaderHasValidIndexToDynamicStringTable(fileHeader, dynamicSectionHeader) );
    assert( map.size >= dynamicSectionHeader.minimumSizeToReadSection() );

    QStringList libraries;
    const SectionHeader dynamicStringTableSectionHeader = extractSectionHeaderAt( map, fileHeader, static_cast<uint16_t>(dynamicSectionHeader.link) );
    const std::vector<DynamicStruct> dynamicSection = extractDynamicSectionForTag(map, fileHeader.ident, dynamicSectionHeader, DynamicSectionTagType::Needed);

    for(const auto & dynamicStruct : dynamicSection){
      const ByteArraySpan charArray = charArrayFromDynamicStringTable(map, dynamicStringTableSectionHeader, dynamicStruct);
      libraries.append( qStringFromUft8UnsignedCharArray(charArray) );
    }

    return libraries;
  }

  /*! \internal Extract the runtime path (DT_RUNPATH)
   *
   * \pre \a map must not be null
   * \pre \a fileHeader must be valid
   * \pre \a dynamicSectionHeader must be a Dynamic section type and be named ".dynamic"
   * \pre \a dynamicSectionHeader must have a valid index to a dynamic string table
   * \pre \a map must be big enough to read the section referenced by \a dynamicSectionHeader
   * \exception ExecutableFileReadError
   */
  inline
  QString extractRunPath(const ByteArraySpan & map, const FileHeader & fileHeader, const SectionHeader & dynamicSectionHeader)
  {
    assert( !map.isNull() );
    assert( fileHeader.ident.isValid() );
    assert( dynamicSectionHeader.sectionType() == SectionType::Dynamic );
    assert( dynamicSectionHeader.name == ".dynamic" );
    assert( sectionHeaderHasValidIndexToDynamicStringTable(fileHeader, dynamicSectionHeader) );
    assert( map.size >= dynamicSectionHeader.minimumSizeToReadSection() );

    const SectionHeader dynamicStringTableSectionHeader = extractSectionHeaderAt( map, fileHeader, static_cast<uint16_t>(dynamicSectionHeader.link) );
    const std::vector<DynamicStruct> dynamicSection = extractDynamicSectionForTag(map, fileHeader.ident, dynamicSectionHeader, DynamicSectionTagType::Runpath);

    if( dynamicSection.empty() ){
      return QString();
    }
    if( dynamicSection.size() != 1 ){
      /// \todo Error
      return QString();
    }

    const ByteArraySpan charArray = charArrayFromDynamicStringTable(map, dynamicStringTableSectionHeader, dynamicSection[0]);

    return qStringFromUft8UnsignedCharArray(charArray);
  }

  /*! \internal
   */
  class ElfFileReaderImpl : public QObject
  {
    Q_OBJECT

   public:

    /*! \internal Common logic to open a file with error handling
     *
     * \pre \a fileInfo must have a file path set
     * \exception ExecutableFileReadError
     */
    void openFile(const QFileInfo & fileInfo)
    {
      assert( !mFile.isOpen() );
      assert( !fileInfo.filePath().isEmpty() );

      if( !fileInfo.exists() ){
        const QString message = tr("file '%1' does not exist").arg( fileInfo.absoluteFilePath() );
        throw ExecutableFileReadError(message);
      }

      mFile.setFileName( fileInfo.absoluteFilePath() );
      if( !mFile.open(QIODevice::ReadOnly) ){
        const QString message = tr("could not open file '%1': %2")
                                .arg( fileInfo.absoluteFilePath(), mFile.errorString() );
        throw ExecutableFileReadError(message);
      }
    }

    /*! \internal
     *
     * \pre a file must allready have been open
     */
    bool isElfFile() noexcept
    {
      assert( mFile.isOpen() );

      if( mFile.size() < 16 ){
        return false;
      }

      const uchar* map = mFile.map(0, 16);
      if( map == nullptr ){
        qDebug() << "map failed";
        return false;
      }

      const Ident ident = extractIdent(map);

      return ident.isValid();
    }

    /*! \internal
     *
     * \todo REMOVE !
     */
    void sandbox()
    {
      assert( mFile.isOpen() );
      
//       if( mFile.size() < 64 ){
//         return;
//       }
//       if( mFile.size() < 200 ){
//         return;
//       }

//       const uchar* map = mFile.map(0, 64);
//       const uchar* map = mFile.map(0, mFile.size());
      const ByteArraySpan map{mFile.map(0, mFile.size()), mFile.size()};
//       if( map == nullptr ){
      if( map.isNull() ){
        qDebug() << "map failed";
        return;
      }

      /// \todo Check e_type (only support executables)
      
      const FileHeader fileHeader = extractFileHeader(map.data);
      
      std::cout << toDebugString(fileHeader).toStdString() << std::endl;
      
      const auto sectionHeaders = extractAllSectionHeaders(map, fileHeader);
      
      std::cout << toDebugString(sectionHeaders).toStdString() << std::endl;

      const SectionHeader sectionNamesStringTableSectionHeader = extractSectionNameStringTableHeader(map, fileHeader);
      if( sectionNamesStringTableSectionHeader.sectionType() == SectionType::Null ){
        qDebug() << "could not find the section names string table section header";
        return;
      }
      
      const SectionHeader dynamicSectionHeader
        = findSectionHeader(map, fileHeader, sectionNamesStringTableSectionHeader, SectionType::Dynamic, ".dynamic");
      if( dynamicSectionHeader.sectionType() == SectionType::Null ){
        qDebug() << "could not find the .dynamic section";
        return;
      }
      
      const QStringList libraries = extractNeededSharedLibraries(map, fileHeader, dynamicSectionHeader);
      std::cout << "Libraries: " << libraries.join(QLatin1String(" ")).toStdString() << std::endl;
      
      const QString runPath = extractRunPath(map, fileHeader, dynamicSectionHeader);
      const QStringList runPathPaths = runPath.split(QLatin1Char(':'));
      std::cout << "RUNPATH: " << runPathPaths.join(QLatin1String("\n")).toStdString() << std::endl;
      
      const SectionHeader dynamicStringTableSectionHeader
        = findSectionHeader(map, fileHeader, sectionNamesStringTableSectionHeader, SectionType::StringTable, ".dynstr");
      if( dynamicStringTableSectionHeader.sectionType() == SectionType::Null ){
        qDebug() << "could not find the .dynstr section";
        return;
      }
      
//       for(const auto & sectionHeader : sectionHeaders){
//         if( (sectionHeader.sectionType() == SectionType::Dynamic) && (sectionHeader.name == ".dynamic") ){
// //           dynamicSectionHeader = sectionHeader;
//         }
//         if( (sectionHeader.sectionType() == SectionType::StringTable) && (sectionHeader.name == ".dynstr") ){
//           dynamicStringTableSectionHeader = sectionHeader;
//         }
//       }
      
//       std::cout << "All of the dynamic section:\n";
//       const auto dynamicSection = extractDynamicSection(map.data, fileHeader.ident, dynamicSectionHeader, dynamicStringTableSectionHeader);
      
//       std::cout << toDebugString(dynamicSection).toStdString() << std::endl;
      
//       const auto neededDynamicSection = extractDynamicSectionForTag(map, fileHeader.ident, dynamicSectionHeader, DynamicSectionTagType::Needed);
//       ///if()
//       std::cout << "NEEDED section:\n";
//       std::cout << toDebugString(neededDynamicSection).toStdString() << std::endl;
      
//       const Ident ident = extractIdent(map);
//       
//       const ObjectFileType objectFileType = extract_e_type(map + 0x10, ident.dataFormat);
//       const Machine machine = extract_e_machine(map + 0x12, ident.dataFormat);
//       
//       std::cout << toDebugString(ident).toStdString() << std::endl;
//       std::cout << "Object file type: " << toDebugString(objectFileType).toStdString() << std::endl;
//       std::cout << "Machine: " << toDebugString(machine).toStdString() << std::endl;
    }

   private:

    QFile mFile;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_ELF_FILE_READER_H
