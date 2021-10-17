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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_DYNAMIC_SECTION_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_DYNAMIC_SECTION_H

#include "StringTable.h"
#include "Ident.h"
#include "Mdt/DeployUtils/ExecutableFileReadError.h"
#include <QString>
#include <QStringList>
#include <QObject>
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cassert>

// #include <iostream>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   *
   * \sa https://refspecs.linuxbase.org/elf/gabi4+/ch5.dynamic.html
   */
  enum class DynamicSectionTagType
  {
    Null = 0,             /*!< Marks the end of the _DYNAMIC array */
    Needed = 1,           /*!< This element holds the string table offset to get the needed library name */
    StringTable = 5,      /*!< This element holds the address to the string table (DT_STRTAB) */
    StringTableSize = 10, /*!< This element holds the size, in bytes, of the string table (DT_STRSZ) */
    SoName = 14,          /*!< This element holds the string table offset to get the sahred object name */
    RPath = 15,           /*!< This element holds the string table offset to get the search path (deprecated) */
    Runpath = 29,         /*!< This element holds the string table offset to get the search path */
    Unknown = 100         /*!< Unknown element (not from the standard) */
  };

  /*! \internal
   *
   * A dynamic section contains a array of ElfN_Dyn, here called DynamicStruct
   *
   * For a 32-bit file, this struct is like:
   * \code
   * struct Elf32_Dyn
   * {
   *   Elf32_Sword d_tag;
   *   union
   *   {
   *     Elf32_Word d_val;
   *     Elf32_Addr d_ptr;
   *   }d_un;
   * };
   * \endcode
   *
   * For a 64-bit file, this struct is like:
   * \code
   * struct Elf64_Dyn
   * {
   *   Elf64_Sxword d_tag;
   *   union
   *   {
   *     Elf64_Xword d_val;
   *     Elf64_Addr d_ptr;
   *   }d_un;
   * };
   * \endcode
   *
   * The various types are:
   * \code
   * Elf32_Sword: int32_t
   * Elf32_Word: uint32_t
   * Elf32_Addr: uint32_t
   *
   * Elf64_Sxword: int64_t
   * Elf64_Xword: uint64_t
   * Elf64_Addr: uint64_t
   * \endcode
   *
   * If replacing the type aliases, we find the following structs.
   *
   * For a 32-bit file, this struct is like:
   * \code
   * struct Elf32_Dyn
   * {
   *   int32_t d_tag;
   *   union
   *   {
   *     uint32_t d_val;
   *     uint32_t d_ptr;
   *   }d_un;
   * };
   * \endcode
   *
   * For a 64-bit file, this struct is like:
   * \code
   * struct Elf64_Dyn
   * {
   *   int64_t d_tag;
   *   union
   *   {
   *     uint64_t d_val;
   *     uint64_t d_ptr;
   *   }d_un;
   * };
   * \endcode
   *
   * \sa https://manpages.debian.org/stretch/manpages/elf.5.en.html
   */
  struct DynamicStruct
  {
    int64_t tag = 0;
    uint64_t val_or_ptr;

    constexpr
    DynamicStruct() noexcept
     : tag(0),
       val_or_ptr(0)
    {
    }

    constexpr
    DynamicStruct(DynamicSectionTagType type) noexcept
     : tag( static_cast<int64_t>(type) ),
       val_or_ptr(0)
    {
    }

    constexpr
    DynamicStruct(const DynamicStruct &) noexcept = default;

    constexpr
    DynamicStruct & operator=(const DynamicStruct &) noexcept = default;

    constexpr
    DynamicStruct(DynamicStruct &&) noexcept = default;

    constexpr
    DynamicStruct & operator=(DynamicStruct &&) noexcept = default;

    constexpr
    DynamicSectionTagType tagType() const noexcept
    {
      switch(tag){
        case 0:
          return DynamicSectionTagType::Null;
        case 1:
          return DynamicSectionTagType::Needed;
        case 5:
          return DynamicSectionTagType::StringTable;
        case 10:
          return DynamicSectionTagType::StringTableSize;
        case 14:
          return DynamicSectionTagType::SoName;
        case 15:
          return DynamicSectionTagType::RPath;
        case 29:
          return DynamicSectionTagType::Runpath;
      }

      return DynamicSectionTagType::Unknown;
    }

    constexpr
    bool isNull() const noexcept
    {
      return tag == 0;
    }

    constexpr
    bool isIndexToStrTab() const noexcept
    {
      switch( tagType() ){
        case DynamicSectionTagType::Needed:
        case DynamicSectionTagType::SoName:
        case DynamicSectionTagType::RPath:
        case DynamicSectionTagType::Runpath:
          return true;
        default:
          break;
      }

      return false;
    }
  };

  /*! \internal
   *
   * Just some hack to have a convenient tr() function available
   */
  class MDT_DEPLOYUTILSCORE_EXPORT DynamicSectionValidator : public QObject
  {
    Q_OBJECT

   public:

    static
    void validateStringTableIndex(DynamicStruct s, const StringTable & stringTable)
    {
      assert( !stringTable.isEmpty() );

      if( !stringTable.indexIsValid(s.val_or_ptr) ){
        const QString msg = tr(
          "a entry in the dynamic section contains a index that is out of bound of the related string table."
          " given index: %1 , string table size: %2"
        ).arg( s.val_or_ptr ).arg( stringTable.byteCount() );
        throw ExecutableFileReadError(msg);
      }
    }
  };

  /*! \internal
   *
   * From the TIS ELF specification v1.2:
   * - Book I, Sections 1-9
   * - Book III, Dynamic Section 2-8
   */
  class DynamicSection
  {
   public:

    /*! \brief STL const iterator
     */
    using const_iterator = std::vector<DynamicStruct>::const_iterator;

    /*! \brief STL iterator
     */
    using iterator = std::vector<DynamicStruct>::iterator;

    /*! \brief Check if this section is null
     */
    bool isNull() const noexcept
    {
      return mSection.empty();
    }

    /*! \brief Get the size of this section in bytes
     *
     * \pre the given class must not be \a ClassNone
     */
    int64_t byteCount(Class c) const noexcept
    {
      assert(c != Class::ClassNone);

      if(c == Class::Class32){
        return 2 * sizeof(uint32_t) * entriesCount();
      }
      assert(c == Class::Class64);

      return 2 * sizeof(uint64_t) * entriesCount();
    }

    /*! \brief Add a entry to this section
     */
    void addEntry(DynamicStruct entry) noexcept
    {
      mSection.push_back(entry);
    }

    /*! \brief Get the count of entries in this section
     */
    int64_t entriesCount() const noexcept
    {
      return static_cast<int64_t>( mSection.size() );
    }

    /*! \brief Get the entry at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < entriesCount() )
     */
    const DynamicStruct & entryAt(int64_t index) const noexcept
    {
      assert( index >= 0 );
      assert( index < entriesCount() );

      return mSection[index];
    }

    /*! \brief Set the string table to this section
     *
     * \pre \a stringTable must not be empty
     * \pre this section must have the DT_STRSZ entry
     */
    void setStringTable(const StringTable & stringTable) noexcept
    {
      assert( !stringTable.isEmpty() );

      mStringTable = stringTable;
      updateStringTableSizeEntry();
    }

    /*! \brief Access the string table of this section
     *
     * Accessing the string table directly is not recommanded.
     * Considere using dedicated getters whenever possible.
     */
    const StringTable & stringTable() const noexcept
    {
      return mStringTable;
    }

    /*! \brief Set the dynamic string table address (DT_STRTAB)
     *
     * \pre the DT_STRTAB entry must exist
     * (DT_STRTAB is mandadtory)
     */
    void setStringTableAddress(uint64_t address) noexcept
    {
      auto it = findMutableEntryForTag(DynamicSectionTagType::StringTable);
      assert( it != end() );

      it->val_or_ptr = address;
    }

    /*! \brief Get the dynamic string table address (DT_STRTAB)
     *
     * \pre the DT_STRTAB entry must exist
     * (DT_STRTAB is mandadtory)
     */
    uint64_t getStringTableAddress() const noexcept
    {
      const auto it = findEntryForTag(DynamicSectionTagType::StringTable);
      assert( it != cend() );

      return it->val_or_ptr;
    }

    /*! \brief Check if the string table size entry exists
     */
    bool containsStringTableSizeEntry() const noexcept
    {
      return findEntryForTag(DynamicSectionTagType::StringTableSize) != cend();
    }

    /*! \brief Get the string table size (DT_STRSZ)
     *
     * \pre the DT_STRSZ entry must exist
     * (DT_STRSZ is mandadtory)
     */
    uint64_t getStringTableSize() const noexcept
    {
      const auto it = findEntryForTag(DynamicSectionTagType::StringTableSize);
      assert( it != cend() );

      return it->val_or_ptr;
    }

    /*! \brief Get the SO name (DT_SONAME)
     *
     * Returns a empty string if this section
     * does not contain a DT_SONAME entry
     * (DT_SONAME is optional).
     *
     * \pre this section must not be null
     * \exception ExecutableFileReadError
     */
    QString getSoName() const
    {
      assert( !isNull() );

      const auto it = findEntryForTag(DynamicSectionTagType::SoName);
      if( it == mSection.cend() ){
        return QString();
      }
      assert( it->tagType() == DynamicSectionTagType::SoName );

      DynamicSectionValidator::validateStringTableIndex(*it, mStringTable);

      return mStringTable.unicodeStringAtIndex(it->val_or_ptr);
    }

    /*! \brief Get the needed shared libraries (DT_NEEDED)
     *
     * Returns a empty list if this section
     * does not contain any DT_NEEDED entry
     * (DT_NEEDED is optional).
     *
     * \pre this section must not be null
     * \exception ExecutableFileReadError
     */
    QStringList getNeededSharedLibraries() const
    {
      assert( !isNull() );

      QStringList libraries;

      const auto f = [this,&libraries](DynamicStruct s){
        if(s.tagType() == DynamicSectionTagType::Needed){
          DynamicSectionValidator::validateStringTableIndex(s, mStringTable);
          libraries.push_back( mStringTable.unicodeStringAtIndex(s.val_or_ptr) );
        }
      };
      std::for_each(mSection.cbegin(), mSection.cend(), f);

      return libraries;
    }

    /*! \brief Check if this section contains the run path (DT_RUNPATH) entry
     */
    bool containsRunPathEntry() const noexcept
    {
      return findRunPathEntry() != cend();
    }

    /*! \brief Get the run path (DT_RUNPATH)
     *
     * Returns a empty string if this section
     * does not contain a DT_RUNPATH entry
     * (DT_RUNPATH is optional).
     *
     * \pre this section must not be null
     * \exception ExecutableFileReadError
     */
    QString getRunPath() const
    {
      assert( !isNull() );

      const auto it = findRunPathEntry();
      if( it == mSection.cend() ){
        return QString();
      }
      assert( it->tagType() == DynamicSectionTagType::Runpath );

      DynamicSectionValidator::validateStringTableIndex(*it, mStringTable);

      return mStringTable.unicodeStringAtIndex(it->val_or_ptr);
    }

    /*! \brief Set the run path (DT_RUNPATH)
     *
     * If \a runPath is a empty string,
     * the run path entry will be removed.
     *
     * \pre this section must not be null
     * \sa removeRunPath()
     * \pre this section must have the DT_STRSZ entry
     */
    void setRunPath(const QString & runPath)
    {
      assert( !isNull() );

      if( runPath.trimmed().isEmpty() ){
        removeRunPath();
        return;
      }

      const auto it = findRunPathEntry();

      if( it != mSection.cend() ){
        const int64_t offset = mStringTable.setUnicodeStringAtIndex(it->val_or_ptr, runPath);
        shiftEntriesIndexingStrTabAfter(*it, offset);
      }else{
        DynamicStruct runPathEntry(DynamicSectionTagType::Runpath);
        runPathEntry.val_or_ptr = mStringTable.appendUnicodeString(runPath);
        addEntry(runPathEntry);
      }

      updateStringTableSizeEntry();
    }

    /*! \brief Remove the run path (DT_RUNPATH) entry
     *
     * \pre this section must not be null
     * \pre this section must have the DT_STRSZ entry
     */
    void removeRunPath() noexcept
    {
      assert( !isNull() );

      const auto it = findRunPathEntry();

      if( it == mSection.cend() ){
        return;
      }

      const int64_t offset = mStringTable.removeStringAtIndex(it->val_or_ptr);
      shiftEntriesIndexingStrTabAfter(*it, offset);

      mSection.erase(it);

      updateStringTableSizeEntry();
    }

    /*! \brief Clear this section
     */
    void clear() noexcept
    {
      mSection.clear();
      mStringTable.clear();
    }

    /*! \brief get the begin iterator
     */
    const_iterator cbegin() const noexcept
    {
      return mSection.cbegin();
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
      return mSection.cend();
    }

    /*! \brief get the begin iterator
     */
    const_iterator end() const noexcept
    {
      return cend();
    }

   private:

    static
    bool isRunPathEntry(DynamicStruct s) noexcept
    {
      return s.tagType() == DynamicSectionTagType::Runpath;
    }

    const_iterator findEntryForTag(DynamicSectionTagType tag) const noexcept
    {
      const auto pred = [tag](DynamicStruct s){
        return s.tagType() == tag;
      };

      return std::find_if(mSection.cbegin(), mSection.cend(), pred);
    }

    iterator findMutableEntryForTag(DynamicSectionTagType tag) noexcept
    {
      const auto pred = [tag](DynamicStruct s){
        return s.tagType() == tag;
      };

      return std::find_if(mSection.begin(), mSection.end(), pred);
    }

    const_iterator findRunPathEntry() const noexcept
    {
      return std::find_if(mSection.cbegin(), mSection.cend(), isRunPathEntry);
    }


    /// \todo use OffsetChange
    /*
     * Initial:
     * string table: \0/tmp\0libA.so\0
     * indexes: 1,6
     *
     * After:
     * string table: \0/path1\0libA.so\0
     * indexes: 1,8
     */
    void shiftEntriesIndexingStrTabAfter(DynamicStruct ref, int64_t offset) noexcept
    {
      assert( ref.isIndexToStrTab() );

      const uint64_t absOffset = static_cast<uint64_t>( std::abs(offset) );

      if(offset < 0){
        for(auto & s : mSection){
          if(s.isIndexToStrTab() && s.val_or_ptr > ref.val_or_ptr){
            s.val_or_ptr -= absOffset;
          }
        }
      }else{
        for(auto & s : mSection){
          if(s.isIndexToStrTab() && s.val_or_ptr > ref.val_or_ptr){
            s.val_or_ptr += absOffset;
          }
        }
      }

//       for(auto & s : mSection){
//         if(s.isIndexToStrTab() && s.val_or_ptr > ref.val_or_ptr){
//           s.val_or_ptr += offset;
//         }
//       }
    }

    void updateStringTableSizeEntry() noexcept
    {
      const auto it = findMutableEntryForTag(DynamicSectionTagType::StringTableSize);
      assert( it != mSection.end() );
      assert( mStringTable.byteCount() > 0 );
      it->val_or_ptr = mStringTable.byteCount();
    }

    std::vector<DynamicStruct> mSection;
    StringTable mStringTable;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_DYNAMIC_SECTION_H
