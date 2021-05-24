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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_HEADER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_FILE_HEADER_H

#include "Ident.h"
#include <cstdint>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal e_type from the header
   */
  enum class ObjectFileType
  {
    None = 0x00,            /*!< An unknown type */
    RelocatableFile = 0x01, /*!< A relocatable file */
    ExecutableFile = 0x02,  /*!< An executable file */
    SharedObject = 0x03,    /*!< A shared object */
    CoreFile = 0x04,        /*!< A core file */
    Unknown = 0x1000        /*!< Not from the standard */
  };

  /*! \internal e_machine from the header
   */
  enum class Machine
  {
    None = 0x00,      /*!< No specific instruction set */
    X86 = 0x03,       /*!< x86 */
    X86_64 = 0x3E,    /*!< AMD x86-64 */
    Unknown = 0xFFFF  /*!< Not from the standard */
  };

  /*! \internal
   */
  struct FileHeader
  {
    Ident ident;
    ObjectFileType type;
    Machine machine;
    uint32_t version;
    uint64_t entry;
    uint64_t phoff;
    uint64_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
    
    /*! \brief Return true if this file has a section name string table
     *
     * \sa https://manpages.debian.org/stretch/manpages/elf.5.en.html
     */
    constexpr
    bool hasSectionNameStringTable() const noexcept
    {
    }
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_HEADER_H
