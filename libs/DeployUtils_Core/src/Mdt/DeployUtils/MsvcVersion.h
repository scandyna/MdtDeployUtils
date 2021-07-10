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
#ifndef MDT_DEPLOY_UTILS_MSVC_VERSION_H
#define MDT_DEPLOY_UTILS_MSVC_VERSION_H

#include "mdt_deployutils_export.h"
#include <cassert>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Represents a MSVC version
   *
   * \sa https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B
   */
  class MDT_DEPLOYUTILS_EXPORT MsvcVersion
  {
   public:

    /*! \brief Construct a null version
     */
    MsvcVersion() noexcept = default;

    /*! \brief Copy construct a version from \a other
     */
    MsvcVersion(const MsvcVersion & other) noexcept = default;

    /*! \brief Copy assign \a other to this version
     */
    MsvcVersion & operator=(const MsvcVersion & other) noexcept = default;

    /*! \brief Move construct a version from \a other
     */
    MsvcVersion(MsvcVersion && other) noexcept = default;

    /*! \brief Move assign \a other to this version
     */
    MsvcVersion & operator=(MsvcVersion && other) noexcept = default;

    /*! \brief Check if this version is null
     */
    bool isNull() const noexcept
    {
      return mMscVer == 0;
    }

    /*! \brief Get the \c _MSC_VER version number
     */
    int mscVer() const noexcept
    {
      return mMscVer;
    }

    /*! \brief Get the Visual Studio product name's year ralated to this MSVC version
     *
     * Will return 0 for unsupported versions.
     *
     * \sa https://en.wikipedia.org/wiki/Microsoft_Visual_Studio
     */
    int toVsProductYearNumber() const noexcept;

    /*! \brief Construct a version from a \c _MSC_VER
     *
     * \pre \a v must be at least 100
     * \sa https://cmake.org/cmake/help/latest/variable/MSVC_VERSION.html
     */
    static
    MsvcVersion fromMscVer(int v) noexcept
    {
      assert(v >= 100);

      MsvcVersion msvcVersion;
      msvcVersion.mMscVer = v;

      return msvcVersion;
    }

   private:

    int mMscVer = 0;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_MSVC_VERSION_H
