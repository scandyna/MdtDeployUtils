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
#include "Exceptions.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include "Mdt/DeployUtils/Impl/ExecutableFileReaderUtils.h"
#include "Mdt/DeployUtils/ExecutableFileReadError.h"
#include <QCoreApplication>
#include <QString>
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
    }else if( header.magicType() == MagicType::Pe32Plus ){
      header.numberOfRvaAndSizes = get32BitValueLE( map.subSpan(108, 4) );
      if( map.size >= 128 ){
        header.importTable = get64BitValueLE( map.subSpan(120, 8) );
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
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Pe{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_PE_FILE_READER_H
