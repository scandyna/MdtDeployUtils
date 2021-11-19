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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_NOTE_SECTION_TABLE_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_NOTE_SECTION_TABLE_H

#include "NoteSection.h"
#include "SectionHeader.h"
#include "SectionHeaderTable.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  struct NoteSectionTableEntry
  {
    SectionHeader header;
    NoteSection section;

    NoteSectionTableEntry() = delete;

    NoteSectionTableEntry(const SectionHeader & _header, const NoteSection & _section) noexcept
     : header(_header),
       section(_section)
    {
    }

    NoteSectionTableEntry(const NoteSectionTableEntry & other) = default;
    NoteSectionTableEntry & operator=(const NoteSectionTableEntry & other) = default;
    NoteSectionTableEntry(NoteSectionTableEntry && other) noexcept = default;
    NoteSectionTableEntry & operator=(NoteSectionTableEntry && other) noexcept = default;
  };

  /*! \internal
   */
  class NoteSectionTable
  {
   public:

    /*! \brief Add a section to this table
     *
     * \pre \a header must be a note section header
     */
    void addSectionFromFile(const SectionHeader & header, const NoteSection & section) noexcept
    {
      assert( isNoteSectionHeader(header) );

      mTable.emplace_back(header, section);
    }

    /*! \brief Get the count of sections in this table
     */
    size_t sectionCount() const noexcept
    {
      return mTable.size();
    }

    /*! \brief Get the section name at \a index
     *
     * \pre \a index must be in a valid range
     */
    const std::string & sectionNameAt(size_t index) const noexcept
    {
      assert( index < sectionCount() );

      return mTable[index].header.name;
    }

    /*! \brief Get the section header at \a index
     *
     * \pre \a index must be in a valid range
     */
    const SectionHeader & sectionHeaderAt(size_t index) const noexcept
    {
      assert( index < sectionCount() );

      return mTable[index].header;
    }

    /*! \brief Get the section at \a index
     *
     * \pre \a index must be in a valid range
     */
    const NoteSection & sectionAt(size_t index) const noexcept
    {
      assert( index < sectionCount() );

      return mTable[index].section;
    }

    /*! \brief Update the section headers regarding \a sectionHeaderTable
     *
     * Should be called if some note sections are moved
     * in the section header table, so that the section headers in this table
     * are up to date.
     */
    void updateSectionHeaders(const std::vector<SectionHeader> & sectionHeaderTable) noexcept
    {
      for(const SectionHeader & shtHeader : sectionHeaderTable){
        if( isNoteSectionHeader(shtHeader) ){
          const auto it = findSectionHeader(shtHeader.name);
          if( it != mTable.cend() ){
            it->header = shtHeader;
          }
        }
      }
    }

    /*! \brief Find the minimum size required to write this table to a file
     */
    int64_t findMinimumSizeToWriteTable() const noexcept
    {
      const auto cmp = [](const NoteSectionTableEntry & a, const NoteSectionTableEntry & b){
        return a.header.minimumSizeToWriteSection() < b.header.minimumSizeToWriteSection();
      };

      const auto it = std::max_element(mTable.cbegin(), mTable.cend(), cmp);
      if( it == mTable.cend() ){
        return 0;
      }

      return it->header.minimumSizeToWriteSection();
    }

   private:

    using iterator = std::vector<NoteSectionTableEntry>::iterator;

    iterator findSectionHeader(const std::string & name) noexcept
    {
      const auto pred = [&name](const NoteSectionTableEntry & entry){
        return entry.header.name == name;
      };

      return std::find_if(mTable.begin(), mTable.end(), pred);
    }

    std::vector<NoteSectionTableEntry> mTable;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_NOTE_SECTION_TABLE_H
