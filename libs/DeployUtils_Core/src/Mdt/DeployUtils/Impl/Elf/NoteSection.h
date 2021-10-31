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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_NOTE_SECTION_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_NOTE_SECTION_H

#include "Algorithm.h"
#include "SectionHeader.h"
#include <cstdint>
#include <string>
#include <vector>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   *
   * \sa https://www.netbsd.org/docs/kernel/elf-notes.html
   *
   * For a list of some xeisting note section,
   * see https://man7.org/linux/man-pages/man5/elf.5.html
   */
  struct NoteSection
  {
    //uint32_t nameSize;
    uint32_t descriptionSize; // [byte]
    uint32_t type;
    std::string name;
    std::vector<uint32_t> description;

    /*! \brief Check if this section is null
     */
    bool isNull() const noexcept
    {
      return name.empty();
    }

    /*! \brief Get the size of this note section
     *
     * \pre this section must not be null
     */
    int64_t byteCountAligned() const noexcept
    {
      assert( !isNull() );

      /*
       * name size: 4 bytes
       * description size: 4 bytes
       * type: 4 bytes
       * name: name size aligned to 4 bytes (must include the null termiation char)
       * description: description size aligned to 4 bytes
       */
      const int64_t nameSize = static_cast<int64_t>( findAlignedSize(static_cast<uint64_t>(name.size()+1), 4) );
      const int64_t descriptionSize = static_cast<int64_t>(description.size() * 4);
      assert( nameSize >= 0 );
      assert( descriptionSize >= 0 );

      return 4 + 4 + 4 + nameSize + descriptionSize;
    }

    /*! \brief Get the minimum size of a note section
     */
    static
    int64_t minimumByteBount() noexcept
    {
      return 4 + 4 + 4;
    }

    /*! \brief Get the maximum size of the name section
     */
    static
    int64_t maximumNameSize(int64_t sectionSize) noexcept
    {
      assert( sectionSize > 0 );

      int64_t result = 0;

      if( sectionSize < minimumByteBount() ){
        return 0;
      }

      result = sectionSize - minimumByteBount();
      assert( result >= 0 );

      return result;
    }
  };

  /*! \internal
   */
  inline
  bool isNoteSectionHeader(const SectionHeader & header) noexcept
  {
    return header.sectionType() == SectionType::Note;
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_NOTE_SECTION_H
