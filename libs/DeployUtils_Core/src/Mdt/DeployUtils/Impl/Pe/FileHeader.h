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
#ifndef MDT_DEPLOY_UTILS_IMPL_PE_FILE_HEADER_H
#define MDT_DEPLOY_UTILS_IMPL_PE_FILE_HEADER_H

#include <cstdint>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Pe{

  /*! \internal
   */
  struct DosHeader
  {
    uint32_t peSignatureOffset = 0;

    constexpr
    bool isNull() const noexcept
    {
      return peSignatureOffset == 0;
    }

    constexpr
    bool seemsValid() const noexcept
    {
      return !isNull();
    }
  };

  /*! \internal
   */
  enum class MachineType
  {
    Unknown = 0x0,        /*!< The content of this field is assumed to be applicable to any machine type */
    Amd64 = 0x8664,       /*!< x64 (x86-64) */
    I386 = 0x14c,         /*!< Intel 386 or later processors and compatible processors */
    NotHandled = 0x9998,  /*!< Not from the standard */
    Null = 0x9999         /*!< Not from the standard */
  };

  /*! \brief Get the minimum size (in bytes) the optional header should be
   *
   * The optional header should at least have the standard fields
   * and the Windows-specific fields.
   *
   * After that comes the data directories.
   * It seems that not all of those are required to be present.
   * 
   * For PE32 image file, the minimum size should be 96.
   * For PE32+ image file, the minimum size should be 112.
   *
   * To make things simple, we assume that at least 1 data directory is present,
   * so we assume a minimum optional header size of 112.
   *
   * \sa https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#optional-header-image-only
   */
  inline
  int64_t minimumOptionalHeaderSize() noexcept
  {
    return 112;
  }

  /*! \internal
   */
  struct CoffHeader
  {
    uint16_t machine = static_cast<uint16_t>(MachineType::Null);
    uint16_t numberOfSections;
    uint32_t timeDateStamp;
    uint32_t pointerToSymbolTable;
    uint32_t numberOfSymbols;
    uint16_t sizeOfOptionalHeader = 0;
    uint16_t characteristics;

    MachineType machineType() const noexcept
    {
      switch(machine){
        case 0:
          return MachineType::Unknown;
        case 0x8664:
          return MachineType::Amd64;
        case 0x14c:
          return MachineType::I386;
        case 0x9999:
          return MachineType::Null;
      }

      return MachineType::NotHandled;
    }

    /*! \brief Check if this header refers to a valid executable image file
     *
     * Flag: IMAGE_FILE_EXECUTABLE_IMAGE
     */
    bool isValidExecutableImage() const noexcept
    {
      return characteristics & 0x0002;
    }

    /*! \brief Check if this header refers to a machine based on a 32-bit word architecture
     *
     * Flag: IMAGE_FILE_32BIT_MACHINE
     */
    bool is32BitWordMachine() const noexcept
    {
      return characteristics & 0x0100;
    }

    /*! \brief Check if this header refers to a DLL image file
     *
     * Flag: IMAGE_FILE_DLL
     */
    bool isDll() const noexcept
    {
      return characteristics & 0x2000;
    }

    /*! \brief Check if this COFF header seems valid
     */
    bool seemsValid() const noexcept
    {
      switch( machineType() ){
        case MachineType::Unknown:
        case MachineType::Null:
        case MachineType::NotHandled:
          return false;
        default:
          break;
      }
      if( !isValidExecutableImage() ){
        return false;
      }
      if( sizeOfOptionalHeader < minimumOptionalHeaderSize() ){
        return false;
      }

      return true;
    }
  };

  /*! \internal
   */
  enum class MagicType
  {
    Unknown,          /*!< Not from the standard */
    Pe32 = 0x10b,     /*!< PE32 32-bit executable */
    Pe32Plus = 0x20b, /*!< PE32+ 64-bit executable */
    RomImage = 0x107  /*!< ROM image */
  };

  /*! \internal
   */
  struct ImageDataDirectory
  {
    uint32_t virtualAddress;
    uint32_t size;

    static
    ImageDataDirectory fromUint64(uint64_t value) noexcept
    {
      ImageDataDirectory directory;

      directory.virtualAddress = static_cast<uint32_t>(value >> 32);
      directory.size = static_cast<uint32_t>(value & 0x00000000FFFFFFFF);

      return directory;
    }
  };

  /*! \internal
   */
  struct OptionalHeader
  {
    uint16_t magic = 0;
    uint32_t numberOfRvaAndSizes = 0;
    uint64_t importTable;

    MagicType magicType() const noexcept
    {
      switch(magic){
        case 0x10b:
          return MagicType::Pe32;
        case 0x20b:
          return MagicType::Pe32Plus;
        case 0x107:
          return MagicType::RomImage;
      }

      return MagicType::Unknown;
    }

    ImageDataDirectory importTableDirectory() const noexcept
    {
      return ImageDataDirectory::fromUint64(importTable);
    }
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Pe{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_PE_FILE_HEADER_H
