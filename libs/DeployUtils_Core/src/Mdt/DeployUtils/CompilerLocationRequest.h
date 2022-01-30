/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_COMPILER_LOCATION_REQUEST_H
#define MDT_DEPLOY_UTILS_COMPILER_LOCATION_REQUEST_H

#include "CompilerLocationType.h"
#include "mdt_deployutilscore_export.h"
#include <QString>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

  /*! \brief DTO to hold compiler location type
   */
  class MDT_DEPLOYUTILSCORE_EXPORT CompilerLocationRequest
  {
   public:

    /*! \brief Set the type of compiler location
     *
     * \pre \a type must be valid
     */
    void setType(CompilerLocationType type) noexcept
    {
      assert(type != CompilerLocationType::Undefined);

      mType = type;
    }

    /*! \brief Set the value of compiler location
     *
     * \pre \a value must not be empty
     */
    void setValue(const QString & value) noexcept
    {
      assert( !value.trimmed().isEmpty() );

      mValue = value;
    }

    /*! \brief Get the type of compiler location
     */
    CompilerLocationType type() const noexcept
    {
      return mType;
    }

    /*! \brief Get the value of compiler location
     */
    const QString & value() const noexcept
    {
      return mValue;
    }

    /*! \brief Check if this location is null
     */
    bool isNull() const noexcept
    {
      return mType == CompilerLocationType::Undefined;
    }

   private:

    CompilerLocationType mType = CompilerLocationType::Undefined;
    QString mValue;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_COMPILER_LOCATION_REQUEST_H
