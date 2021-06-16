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
#ifndef MDT_DEPLOY_UTILS_IMPL_EXECUTABLE_FILE_READER_UTILS_H
#define MDT_DEPLOY_UTILS_IMPL_EXECUTABLE_FILE_READER_UTILS_H

#include "ByteArraySpan.h"
#include "NotNullTerminatedStringError.h"
#include <QtEndian>
#include <QString>
#include <QCoreApplication>
#include <initializer_list>
#include <cstdint>
#include <algorithm>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{

  /*! \internal
   */
  inline
  QString tr(const char *sourceText) noexcept
  {
    assert( sourceText != nullptr );

    return QCoreApplication::translate("Mdt::DeployUtils::Impl::ExecutableFileReaderUtils", sourceText);
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
   * \pre \a charArray must not be null
   * \exception NotNullTerminatedStringError
   */
  inline
  QString qStringFromUft8UnsignedCharArray(const ByteArraySpan & charArray)
  {
    assert( !charArray.isNull() );

    if( !containsEndOfString(charArray) ){
      const QString message = tr("failed to extract a string from a region (end of string not found)");
      throw NotNullTerminatedStringError(message);
    }

    return QString::fromUtf8( reinterpret_cast<const char*>(charArray.data) );
  }

  /*! \internal
   *
   * \pre \a start must not be a nullptr
   * \pre \a count must be >= 0
   */
  inline
  bool arraysAreEqual(const unsigned char * const start, int64_t count, std::initializer_list<unsigned char> reference) noexcept
  {
    assert( start != nullptr );
    assert( count >= 0 );

    const auto first = start;
    const auto last = start + count;

    return std::equal( first, last, reference.begin(), reference.end() );
  }

  /*! \internal
   *
   * \pre \a array must not be null
   * \pre \a array size must be at least 2 bytes
   */
  inline
  uint16_t get16BitValueLE(const ByteArraySpan & array) noexcept
  {
    assert( !array.isNull() );
    assert( array.size >= 2 );

    return qFromLittleEndian<uint16_t>(array.data);
  }

  /*! \internal
   *
   * \pre \a array must not be null
   * \pre \a array size must be at least 4 bytes
   */
  inline
  uint32_t get32BitValueLE(const ByteArraySpan & array) noexcept
  {
    assert( !array.isNull() );
    assert( array.size >= 4 );

    return qFromLittleEndian<uint32_t>(array.data);
  }

  /*! \internal
   *
   * \pre \a array must not be null
   * \pre \a array size must be at least 8 bytes
   */
  inline
  uint64_t get64BitValueLE(const ByteArraySpan & array) noexcept
  {
    assert( !array.isNull() );
    assert( array.size >= 8 );

    return qFromLittleEndian<uint64_t>(array.data);
  }

}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_EXECUTABLE_FILE_READER_UTILS_H
