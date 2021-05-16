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

#include "Header.h"
#include "Mdt/DeployUtils/ExecutableFileReadError.h"
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QObject>
#include <QtEndian>
#include <initializer_list>
#include <cstdint>

// #include <iterator>

#include <algorithm>
#include <cassert>

#include "Debug.h"
#include <QDebug>
#include <iostream>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{


  /*! \internal
   *
   * \pre \a dataFormat must be valid
   * \pre \a s must not be a nullptr
   * \pre \a the array referenced by \a a must ba at least 2 bytes long
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
   * \a ident will be used,
   * to know the endianness of the file.
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
      
      if( mFile.size() < 0x40 ){
        return;
      }

      const uchar* map = mFile.map(0, 0x40);
      if( map == nullptr ){
        qDebug() << "map failed";
        return;
      }

      const Ident ident = extractIdent(map);
      
      const ObjectFileType objectFileType = extract_e_type(map + 0x10, ident.dataFormat);
      
      std::cout << toDebugString(ident).toStdString() << std::endl;
      std::cout << "Object file type: " << toDebugString(objectFileType).toStdString() << std::endl;

    }

   private:

    QFile mFile;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_ELF_FILE_READER_H
