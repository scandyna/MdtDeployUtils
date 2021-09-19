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

// #include <iostream>

/// \todo remove
// inline
// void debugStringTable(const std::vector<char> & table)
// {
//   for(char c : table){
//     if(c == 0){
//       std::cout << "\\0";
//     }else{
//       std::cout << c;
//     }
//   }
//   std::cout << std::endl;
// }

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
     * \pre \a charArray must not be empty
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

    /*! \brief STL const iterator
     */
    using const_iterator = std::vector<char>::const_iterator;

    /*! \brief Construct a empty string table
     */
    StringTable() noexcept
     : mTable(1, '\0')
    {
    }

    /*! \brief Copy construct a string table from \a other
     */
    StringTable(const StringTable & other) = default;

    /*! \brief Copy assign \a other to this string table
     */
    StringTable & operator=(const StringTable & other) = default;

    /*! \brief Move construct a string table from \a other
     */
    StringTable(StringTable && other) noexcept = default;

    /*! \brief Move assign \a other to this string table
     */
    StringTable & operator=(StringTable && other) noexcept = default;

    /*! \brief Get the size of this table in bytes
     */
    int64_t byteCount() const noexcept
    {
      return static_cast<int64_t>( mTable.size() );
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

    /*! \brief Clear this string table
     */
    void clear() noexcept
    {
      mTable.clear();
      mTable.push_back('\0');
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

    /*! \brief Add \a str to the end of this table
     *
     * Returns the index to the \a str once added
     *
     * \note here the name \a index is used,
     *  which is the one used in the standrad.
     *  In reality, it is more a offset from the start of the table
     *
     * \pre \a str must not be empty
     */
    uint64_t appendString(const std::string & str)
    {
      assert( !str.empty() );

      const uint64_t index = mTable.size();

      mTable.insert( mTable.cend(), str.cbegin(), str.cend() );
      mTable.push_back('\0');

      return index;
    }

    /*! \brief Remove the string at \a index from this table
     *
     * \code
     * int64_t offset = 0;
     * StringTable table;
     *
     * // string table: \0libA.so\0
     * offset = table.removeStringAtIndex(1);
     * // offset: -8
     * // string table: \0
     * \endcode
     *
     * \note here the name \a index is used,
     *  which is the one used in the standrad.
     *  In reality, it is more a offset from the start of the table
     *
     * \pre \a index must be > 0 (the first bye must allways be a null char in this table)
     * \pre \a index must be < byteCount()
     */
    int64_t removeStringAtIndex(uint64_t index) noexcept
    {
      assert( index > 0 );

      const int64_t sIndex = static_cast<int64_t>(index);
      assert( sIndex < byteCount() );

      const auto first = mTable.cbegin() + sIndex;
      const auto last = std::find(first, mTable.cend(), '\0') + 1;
      if(first == last){
        return 0;
      }

      mTable.erase(first, last);

      return first - last;
    }

    /*! \brief Set the string at \a index to \a str in this table
     *
     * \code
     * int64_t offset = 0;
     * StringTable table;
     *
     * // string table: \0
     * offset = table.setStringAtIndex(1, "libA.so");
     * // offset: 7
     *
     * // string table: \0libA.so\0
     * offset = table.setStringAtIndex(1, "libB.so");
     * // offset: 0
     *
     * // string table: \0libB.so\0
     * offset = table.setStringAtIndex(1, "libQt5Core.so");
     * // offset: 6
     *
     * // string table: \0libQt5Core.so\0
     * offset = table.setStringAtIndex(1, "libC.so");
     * // offset: -6
     * \endcode
     *
     * \note here the name \a index is used,
     *  which is the one used in the standrad.
     *  In reality, it is more a offset from the start of the table
     *
     * \pre \a index must be > 0 (the first bye must allways be a null char in this table)
     * \pre \a index must be <= byteCount() (in bound of this table, or 1 byte after the last byte, but not above)
     * \pre \a str must not be empty
     */
    int64_t setStringAtIndex(uint64_t index, const std::string & str) noexcept
    {
      assert( !str.empty() );
      assert( index > 0 );

      const int64_t sIndex = static_cast<int64_t>(index);
      assert( sIndex <= byteCount() );

      const bool addNewString = ( sIndex >= byteCount() );

      std::string currentString;
      if( !addNewString ){
        currentString = stringAtIndex(index);
      }

      const int64_t strSize = static_cast<int64_t>( str.size() );
      const int64_t currentStringSize = static_cast<int64_t>( currentString.size() );
      const int64_t offset = strSize - currentStringSize;

      /*
       * \0
       * \0NewString\0
       *
       * \0Old\0
       * \0NewString\0
       *
       * \0OldString\0
       * \0NewString\0
       *
       * \0OldString\0
       * \0New\0
       */
      if(offset > 0){
        const auto pos = mTable.cbegin() + sIndex + currentStringSize;
        mTable.insert(pos, static_cast<size_t>(offset), '\0');

        if(addNewString){
          mTable.push_back('\0');
        }
      }

      if(offset < 0){
        const auto first = mTable.cbegin() + sIndex + strSize;
        const auto last = mTable.cbegin() + sIndex + currentStringSize;
        mTable.erase(first, last);
      }

      auto it = mTable.begin() + sIndex;
      std::copy(str.cbegin(), str.cend(), it);

      return offset;
    }

    /*! \brief Get the string at \a index in this table
     *
     * The ELF specification seems not to say anything about unicode encoding.
     * Because ELF is Unix centric, it is assumed that \a charArray
     * represents a UTF-8 string.
     * (note: using platform specific encoding can be problematic
     *        for cross-compilation)
     *
     * \note here the name \a index is used,
     *  which is the one used in the standrad.
     *  In reality, it is more a offset from the start of the table
     *
     * \pre \a index must be valid
     * \sa indexIsValid()
     */
    QString unicodeStringAtIndex(uint64_t index) const noexcept
    {
      assert( indexIsValid(index) );

      return QString::fromUtf8(mTable.data() + index);
    }

    /*! \brief Add \a str to the end of this table
     *
     * Returns the index to the \a str once added
     *
     * \note here the name \a index is used,
     *  which is the one used in the standrad.
     *  In reality, it is more a offset from the start of the table
     *
     * \pre \a str must not be empty
     */
    uint64_t appendUnicodeString(const QString & str)
    {
      assert( !str.isEmpty() );

      return appendString( str.toStdString() );
    }

    /*! \brief Set the string at \a index to \a str in this table
     *
     * \note here the name \a index is used,
     *  which is the one used in the standrad.
     *  In reality, it is more a offset from the start of the table
     *
     * \sa setStringAtIndex()
     *
     * \pre \a index must be > 0 (the first bye must allways be a null char in this table)
     * \pre \a index must be <= byteCount() (in bound of this table, or 1 byte after the last byte, but not above)
     * \pre \a str must not be empty
     */
    int64_t setUnicodeStringAtIndex(uint64_t index, const QString & str) noexcept
    {
      assert( !str.isEmpty() );
      assert( index > 0 );
      assert( static_cast<int64_t>(index) <= byteCount() );

      return setStringAtIndex( index, str.toStdString() );
    }

    /*! \brief get the begin iterator
     */
    const_iterator cbegin() const noexcept
    {
      return mTable.cbegin();
    }

    /*! \brief get the begin iterator
     */
    const_iterator begin() const noexcept
    {
      return cbegin();
    }

    /*! \brief get the begin iterator
     */
    const_iterator cend() const noexcept
    {
      return mTable.cend();
    }

    /*! \brief get the begin iterator
     */
    const_iterator end() const noexcept
    {
      return cend();
    }

    /*! \brief Extract a copy of a string table from \a map
     *
     * \pre \a charArray must not be empty
     * \exception StringTableError
     */
    static
    StringTable fromCharArray(const ByteArraySpan & charArray)
    {
      assert( !charArray.isNull() );
      assert( charArray.size > 0 );

      validateStringTableArray(charArray);

      return StringTable(charArray);
    }

   private:

    StringTable(const ByteArraySpan & charArray)
    {
      mTable.reserve( static_cast<size_t>(charArray.size) );
      std::copy( charArray.cbegin(), charArray.cend(), std::back_inserter(mTable) );
    }

    std::vector<char> mTable;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_STRING_TABLE_H
