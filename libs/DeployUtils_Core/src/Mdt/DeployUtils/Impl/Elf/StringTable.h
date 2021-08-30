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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_STRING_TABLE_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_STRING_TABLE_H

#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include "Mdt/DeployUtils/Impl/StringTableError.h"
#include "mdt_deployutilscore_export.h"
#include <string>
#include <vector>
#include <QString>
#include <QObject>
#include <cstdint>
#include <algorithm>
#include <iterator>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   *
   * Just some hack to have a convenient tr() function available
   */
  class MDT_DEPLOYUTILSCORE_EXPORT StringTableValidator : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief
     *
     * From the TIS ELF specification v1.2:
     * - Book I, String Table 1-18
     *
     * \pre \a charArray must not be null
     * \exception StringTableError
     */
    static
    void validateStringTableArray(const ByteArraySpan & charArray)
    {
      assert( !charArray.isNull() );
      assert( charArray.size > 0 );

      if( charArray.data[0] != '\0' ){
        const QString msg = tr("string table does not begin with a null byte");
        throw StringTableError(msg);
      }

      if( charArray.data[charArray.size-1] != '\0' ){
        const QString msg = tr("string table is not null terminated");
        throw StringTableError(msg);
      }
    }
  };

  /*! \internal
   *
   * From the TIS ELF specification v1.2:
   * - Book I, String Table 1-18
   *
   * \pre \a charArray must not be null
   * \exception StringTableError
   */
  inline
  void validateStringTableArray(const ByteArraySpan & charArray)
  {
    assert( !charArray.isNull() );

    StringTableValidator::validateStringTableArray(charArray);
  }

  /*! \internal
   *
   * From the TIS ELF specification v1.2:
   * - Book I, String Table 1-18
   */
  class StringTable
  {
   public:

    /*! \brief Get the size of this table in bytes
     */
    int64_t byteCount() const noexcept
    {
      return mTable.size();
    }

    /*! \brief Check if this string table is empty
     *
     * \note A empty string table can be accessed at index 0
     * \sa stringAtIndex()
     * \sa unicodeStringAtIndex()
     */
    bool isEmpty() const noexcept
    {
      return byteCount() <= 1;
    }

    /*! \brief Check if \a index is in bound in this string table
     */
    bool indexIsValid(uint64_t index) const noexcept
    {
      return static_cast<int64_t>(index) < byteCount();
    }

    /*! \brief Get the string at \a index in this table
     *
     * \note here the name \a index is used,
     *  which is the one used in the standrad.
     *  In reality, it is more a offset from the start of the table
     *
     * \pre \a index must be valid
     * \sa indexIsValid()
     */
    std::string stringAtIndex(uint64_t index) const noexcept
    {
      assert( indexIsValid(index) );

      return std::string(mTable.data() + index);
    }

    /*! \brief Get the string at \a index in this table
     *
     * The ELF specification seems not to say anything about unicode encoding.
     * Because ELF is Unix centric, it is assumed that \a charArray
     * represents a UTF-8 string.
     * (note: using platform specific encoding can be problematic
     *        for cross-compilation)
     *
     * \pre \a index must be valid
     * \sa indexIsValid()
     */
    QString unicodeStringAtIndex(uint64_t index) const noexcept
    {
      assert( indexIsValid(index) );

      return QString::fromUtf8(mTable.data() + index);
    }

    /*! \brief Extract a copy of a string table from \a map
     *
     * \pre \a charArray must not be null
     * \exception StringTableError
     */
    static
    StringTable fromCharArray(const ByteArraySpan & charArray)
    {
      assert( !charArray.isNull() );

      validateStringTableArray(charArray);

      StringTable stringTable;

      auto & tableRef = stringTable.mTable;
      tableRef.reserve(charArray.size);
      std::copy( charArray.cbegin(), charArray.cend(), std::back_inserter(tableRef) );

      return stringTable;
    }

   private:

    std::vector<char> mTable;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_STRING_TABLE_H
