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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_OFFSET_CHANGES_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_FILE_OFFSET_CHANGES_H

#include "DynamicSection.h"
#include "StringTable.h"
#include "Ident.h"
#include <cstdint>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  class FileOffsetChanges
  {
   public:

    /*! \brief Set the original sizes regarding \a dynamicSection and its related string table
     *
     * \pre the given class must not be \a ClassNone
     */
    void setOriginalSizes(const DynamicSection & dynamicSection, Class c) noexcept
    {
      assert(c != Class::ClassNone);

      mOriginalDynamicSectionByteCount = dynamicSection.byteCount(c);
      mOriginalDynamicStringTableByteCount = dynamicSection.stringTable().byteCount();
      mClass = c;
    }

    /*! \brief Get the offset of the changes applyed to the dynamic section (in bytes)
     *
     * For example, if a entry was added since the call to setOriginalSizes(),
     * 16 will be returned for a 64-bit file.
     *
     * If a entry was removed, -16 will be returned for a 64-bit file.
     */
    int64_t dynamicSectionChangesOffset(const DynamicSection & dynamicSection) const noexcept
    {
      assert(mClass != Class::ClassNone);

      return dynamicSection.byteCount(mClass) - mOriginalDynamicSectionByteCount;
    }

    /*! \brief Get the offset of changes applyed to the dynamic string table related to \a dynamicSection (in bytes)
     */
    int64_t dynamicStringTableChangesOffset(const DynamicSection & dynamicSection) const noexcept
    {
      assert(mClass != Class::ClassNone);

      return dynamicSection.stringTable().byteCount() - mOriginalDynamicStringTableByteCount;
    }

    /*! \brief Get the offset of changes applyed to the dynamic section and its related string table
     */
    int64_t globalChangesOffset(const DynamicSection & dynamicSection) const noexcept
    {
      assert(mClass != Class::ClassNone);

      return dynamicSectionChangesOffset(dynamicSection) + dynamicStringTableChangesOffset(dynamicSection);
    }

   private:

    int64_t mOriginalDynamicSectionByteCount = 0;
    int64_t mOriginalDynamicStringTableByteCount = 0;
    Class mClass = Class::ClassNone;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_OFFSET_CHANGES_H
