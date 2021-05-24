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
#include <QObject>
#include <QtEndian>
#include <initializer_list>
#include <cstdint>

// #include <iterator>

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
  struct ByteArraySpan
  {
    const unsigned char *data = nullptr;
    uint64_t size = 0;
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

//     if( ident._class == Class::Class32 ){
//       return getWord(s, ident.dataFormat);
//     }
// 
//     assert( ident._class == Class::Class64 );
// 
//     /*
//      * using qFromBigEndian<usint64_t>(s);
//      * or qFromLittleEndian<usint64_t>(s);
//      * does not work (linker error),
//      * so use Qt defined quint64
//      */
//     if( ident.dataFormat == DataFormat::Data2MSB ){
//       return qFromBigEndian<quint64>(s);
//     }
// 
//     assert( ident.dataFormat == DataFormat::Data2LSB );
// 
//     return qFromLittleEndian<quint64>(s);
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

  /*! \internal Get a string from a array of unsigned characters
   *
   * Will get the characters from \a charArray until
   * a null char is encountered, or maxLength has been reached.
   *
   * Each character will be casted to a (signed) char,
   * meaning that values > 127 are invalid.
   *
   * \todo document what to do with invalid chars
   * \todo see PE format, maybe this will go to some algorithm header later
   *
   * \pre \a charArray must not be a nullptr
   * \pre \a charArray must be at least \a maxLength long
   * \pre \a maxLength must be > 0 (\todo should accept 0 length, and adapt tests)
   */
  inline
  std::string stringFromUnsignedCharArray(const unsigned char * const charArray, int maxLength) noexcept
  {
    assert( charArray != nullptr );
    assert( maxLength > 0 );

//     qDebug() << "char array: " << (char)charArray[0] << (char)charArray[1];
//     qDebug() << "char array casted: " << reinterpret_cast<const char*>(charArray);
//     std::cout << "str: " << std::string( reinterpret_cast<const char*>(charArray) ) << std::endl;
    
    return std::string( reinterpret_cast<const char*>(charArray) );
  }

  /*! \internal
   *
   * \pre \a stringTableSectionHeader must be the string table section header
   */
  inline
  void setSectionHeaderName(const unsigned char * const map, const SectionHeader & stringTableSectionHeader,
                            SectionHeader & sectionHeader) noexcept
  {
    assert( stringTableSectionHeader.sectionType() == SectionType::StringTable );
//     assert( sectionHeader.sectionType() != SectionType::StringTable );
    
    const unsigned char *it = map + stringTableSectionHeader.offset + sectionHeader.nameIndex;
    
    qDebug() << "set name, string table offset: " << stringTableSectionHeader.offset
             << ", name index: " << sectionHeader.nameIndex
             << " offset: " << stringTableSectionHeader.offset + sectionHeader.nameIndex
             << ", name: " << (char)it[0] << (char)it[1];

    sectionHeader.name = stringFromUnsignedCharArray(map + stringTableSectionHeader.offset + sectionHeader.nameIndex, 100);
//     sectionHeader.name = std::string( reinterpret_cast<const char*>(map + stringTableSectionHeader.offset + sectionHeader.nameIndex) );
  }

  /*! \internal
   *
   * \pre \a stringTableSectionHeader must be the string table section header
   */
  inline
  void setSectionHeadersName(const unsigned char * const map, const SectionHeader & stringTableSectionHeader,
                             std::vector<SectionHeader> & sectionHeaders) noexcept
  {
    assert( stringTableSectionHeader.sectionType() == SectionType::StringTable );

    for(auto & sectionHeader : sectionHeaders){
      setSectionHeaderName(map, stringTableSectionHeader, sectionHeader);
    }
  }
  /*! \internal
   *
   * \pre \a array must not be a nullptr
   * \pre the array referenced by \a array must have at least
   *     40 bytes for 32-bit files, 64 bytes for 64-bit files
   * \pre \a ident must be valid
   * \note this function will not set the section header name
   * \sa setSectionHeaderName()
   */
  inline
  SectionHeader sectionHeaderFromArray(const unsigned char * const array, const Ident & ident) noexcept
  {
    assert( array != nullptr );
    assert( ident.isValid() );

    SectionHeader sectionHeader;
    const unsigned char *it = array;

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

    return sectionHeader;
  }

  /*! \internal
   *
   * \note this function will not set the section header name
   * \sa setSectionHeaderName()
   */
  inline
  SectionHeader extractSectionHeaderAt(const unsigned char * const map, const FileHeader & fileHeader, uint16_t index) noexcept
  {
    const unsigned char *it = map + fileHeader.shoff + index * fileHeader.shentsize;
    
//     qDebug() << "extract at, shoff: " << fileHeader.shoff << ", index: " << index << ", shentsize: " << fileHeader.shentsize << ", map address: " << (quint64)it;

    return sectionHeaderFromArray(it, fileHeader.ident);
  }

  /*! \internal
   */
  inline
  SectionHeader extractSectionNameStringTableHeader(const unsigned char * const map, const FileHeader & fileHeader) noexcept
  {
    return extractSectionHeaderAt(map, fileHeader, fileHeader.shstrndx);
  }

  /*! \internal
   */
  inline
  std::vector<SectionHeader> extractAllSectionHeaders(const unsigned char * const map, const FileHeader & fileHeader) noexcept
  {
    std::vector<SectionHeader> sectionHeaders;

//     const unsigned char *it = map + fileHeader.shoff;
    const uint16_t sectionHeaderCount = fileHeader.shnum;
//     const uint16_t sectionHeaderSize = fileHeader.shentsize;

    for(uint16_t i = 0; i < sectionHeaderCount; ++i){
      sectionHeaders.emplace_back( extractSectionHeaderAt(map, fileHeader, i) );
//       it += sectionHeaderSize;
    }

    const SectionHeader stringTableSectionHeader = extractSectionNameStringTableHeader(map, fileHeader);
    setSectionHeadersName(map, stringTableSectionHeader, sectionHeaders);

    return sectionHeaders;
  }

  /*! \internal
   *
   * \pre \a map must not be a nullptr
   * \pre \a ident must be valid
   * \pre \a dynamicSectionHeader must be a Dynamic section type and be named ".dynamic"
   * \pre \a dynamicStringTableSectionHeader must be a string table header and be named ".dynstr"
   */
  inline
  std::vector<DynamicStruct> extractDynamicSection(const unsigned char * const map, const Ident & ident,
                                                   const SectionHeader & dynamicSectionHeader,
                                                   const SectionHeader & dynamicStringTableSectionHeader) noexcept
  {
    assert( map != nullptr );
    assert( ident.isValid() );
    assert( dynamicSectionHeader.sectionType() == SectionType::Dynamic );
    assert( dynamicSectionHeader.name == ".dynamic" );
    assert( dynamicStringTableSectionHeader.sectionType() == SectionType::StringTable );
    assert( dynamicStringTableSectionHeader.name == ".dynstr" );

    std::vector<DynamicStruct> dynamicSection;

    const unsigned char * first = map + dynamicSectionHeader.offset;
    const unsigned char * const last = first + dynamicSectionHeader.size;

//     qDebug() << "offset: 0x" << QString::number(dynamicSectionHeader.offset, 16);

    while(first < last){
      DynamicStruct sectionStruct;
      
//       qDebug() << "first: " << first << " , offset: 0x" << QString::number(first - dynamicSection.offset);

      sectionStruct.tag = getSignedNWord(first, ident);
      advance4or8bytes(first, ident);

      sectionStruct.val_or_ptr = getNWord(first, ident);
      advance4or8bytes(first, ident);
      
      if( sectionStruct.tagType() == DynamicSectionTagType::Needed ){
        std::cout << "name: " << stringFromUnsignedCharArray(map + dynamicStringTableSectionHeader.offset + sectionStruct.val_or_ptr, 50) << std::endl;
      }

      if( sectionStruct.tagType() == DynamicSectionTagType::SoName ){
        std::cout << "so name: " << stringFromUnsignedCharArray(map + dynamicStringTableSectionHeader.offset + sectionStruct.val_or_ptr, 50) << std::endl;
      }

      if( sectionStruct.tagType() == DynamicSectionTagType::RPath ){
        std::cout << "RPATH: " << stringFromUnsignedCharArray(map + dynamicStringTableSectionHeader.offset + sectionStruct.val_or_ptr, 50) << std::endl;
      }

      if( sectionStruct.tagType() == DynamicSectionTagType::Runpath ){
        std::cout << "RUNPATH: " << stringFromUnsignedCharArray(map + dynamicStringTableSectionHeader.offset + sectionStruct.val_or_ptr, 500) << std::endl;
      }

      dynamicSection.push_back(sectionStruct);
    }

    return dynamicSection;
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
      const uchar* map = mFile.map(0, mFile.size());
      if( map == nullptr ){
        qDebug() << "map failed";
        return;
      }

      const FileHeader fileHeader = extractFileHeader(map);
      
      std::cout << toDebugString(fileHeader).toStdString() << std::endl;
      
      const auto sectionHeaders = extractAllSectionHeaders(map, fileHeader);
      
      std::cout << toDebugString(sectionHeaders).toStdString() << std::endl;

      SectionHeader dynamicSectionHeader;
      SectionHeader dynamicStringTableSectionHeader;
      
      for(const auto & sectionHeader : sectionHeaders){
        if( (sectionHeader.sectionType() == SectionType::Dynamic) && (sectionHeader.name == ".dynamic") ){
          dynamicSectionHeader = sectionHeader;
        }
        if( (sectionHeader.sectionType() == SectionType::StringTable) && (sectionHeader.name == ".dynstr") ){
          dynamicStringTableSectionHeader = sectionHeader;
        }
      }
      
      const auto dynamicSections = extractDynamicSection(map, fileHeader.ident, dynamicSectionHeader, dynamicStringTableSectionHeader);
      
      std::cout << toDebugString(dynamicSections).toStdString() << std::endl;
      
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
