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
#ifndef MDT_DEPLOY_UTILS_IMPL_PE_IMPORT_DIRECTORY_H
#define MDT_DEPLOY_UTILS_IMPL_PE_IMPORT_DIRECTORY_H

#include <QLatin1String>
#include <cstdint>
#include <vector>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Pe{

  /*! \internal
   */
  struct ImportDirectory
  {
    uint32_t nameRVA = 0;

    /*! \brief Check if this directory is null
     *
     * This directory is null if all values are 0
     *
     * \sa https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#the-idata-section
     */
    bool isNull() const noexcept
    {
      return nameRVA == 0;
    }
  };

  /*! \internal
   */
  using ImportDirectoryTable = std::vector<ImportDirectory>;

  /*! \internal
   */
  struct DelayLoadDirectory
  {
    uint32_t attributes = 0;
    uint32_t nameRVA = 0;

    /*! \brief Check if this directory is null
     *
     * This directory is null if all values are 0
     *
     * \sa https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#the-idata-section
     */
    bool isNull() const noexcept
    {
      if(attributes != 0){
        return false;
      }
      if(nameRVA != 0){
        return false;
      }
      return true;
    }
  };

  /*! \internal
   */
  using DelayLoadTable = std::vector<DelayLoadDirectory>;

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Pe{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_PE_IMPORT_DIRECTORY_H
