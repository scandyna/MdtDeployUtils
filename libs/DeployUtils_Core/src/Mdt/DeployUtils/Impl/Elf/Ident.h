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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_IDENT_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_IDENT_H

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  enum class Class
  {
    ClassNone,  /*!< */
    Class32,    /*!< */
    Class64     /*!< */
  };

  /*! \internal
   */
  enum class DataFormat
  {
    DataNone, /*!< Unknown data format */
    Data2LSB, /*!< Two's complement, little-endian */
    Data2MSB  /*!< Two's complement, big-endian */
  };

  /*! \internal
   *
   * \sa https://refspecs.linuxfoundation.org/elf/gabi4+/ch4.eheader.html#elfid
   */
  enum class OsAbiType
  {
    SystemV = 0x00,       /*!< UNIX System V ABI */
    Linux = 0x03,         /*!< */
    Unknown = 63,         /*!< Not from the standard */
    ArchitectureSpecific  /*!< Architecture-specific value range 64-255 */
  };

  /*! \internal
   */
  struct Ident
  {
    bool hasValidElfMagicNumber;
    Class _class;
    DataFormat dataFormat;
    unsigned char version;
    unsigned char osabi;
    unsigned char abiversion;

    /*! \brief Returns the OS ABI deduced from osabi value
     */
    constexpr
    OsAbiType osAbiType() const noexcept
    {
      if( osabi == 0 ){
        return OsAbiType::SystemV;
      }
      if( osabi == 3 ){
        return OsAbiType::Linux;
      }
      if( osabi >= 64 ){
        return OsAbiType::ArchitectureSpecific;
      }

      return OsAbiType::Unknown;
    }

    /*! \brief Returns true if this ident is valid
     */
    constexpr
    bool isValid() const noexcept
    {
      if( !hasValidElfMagicNumber ){
        return false;
      }
      if( _class == Class::ClassNone ){
        return false;
      }
      if( dataFormat == DataFormat::DataNone ){
        return false;
      }
      if( version != 1 ){
        return false;
      }
      if( osAbiType() == OsAbiType::Unknown ){
        return false;
      }

      return true;
    }
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_IDENT_H
