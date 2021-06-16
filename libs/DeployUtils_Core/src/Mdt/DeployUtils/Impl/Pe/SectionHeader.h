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
#ifndef MDT_DEPLOY_UTILS_IMPL_PE_SECTION_HEADER_H
#define MDT_DEPLOY_UTILS_IMPL_PE_SECTION_HEADER_H

#include <QString>
#include <QLatin1Char>
#include <cstdint>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Pe{

  /*! \internal
   */
  struct SectionHeader
  {
    QString name;
    uint32_t virtualSize = 0;
    uint32_t virtualAddress = 0;
    uint32_t sizeOfRawData = 0;
    uint32_t pointerToRawData = 0;

    bool seemsValid() const noexcept
    {
      if( name.isEmpty() ){
        return false;
      }
      if( name[0] == QLatin1Char('/') ){
        return false;
      }
      if( virtualSize == 0 ){
        return false;
      }
      if( sizeOfRawData == 0 ){
        return false;
      }
      if( pointerToRawData == 0 ){
        return false;
      }
      if( virtualAddress < pointerToRawData ){
        return false;
      }

      return true;
    }

    /*! \brief Check if \a rva is a valid address
     *
     * \pre this header must be valid
     */
    bool rvaIsValid(uint32_t rva) const noexcept
    {
      assert( seemsValid() );

      return rva >= virtualAddress_PointerToRawData_Offset();
    }

    /*! \brief Get a file offset from \a rva
     *
     * \pre this header must be valid
     * \pre \a rva must be valid
     */
    int64_t rvaToFileOffset(uint32_t rva) const noexcept
    {
      assert( seemsValid() );
      assert( rvaIsValid(rva) );

      return rva - virtualAddress_PointerToRawData_Offset();
    }

   private:

    inline
    uint32_t virtualAddress_PointerToRawData_Offset() const noexcept
    {
      return virtualAddress - pointerToRawData;
    }
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Pe{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_PE_SECTION_HEADER_H
