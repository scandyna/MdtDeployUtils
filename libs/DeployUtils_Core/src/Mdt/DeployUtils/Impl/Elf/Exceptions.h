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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_EXCEPTIONS_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_EXCEPTIONS_H

#include "Mdt/DeployUtils/QRuntimeError.h"
#include "mdt_deployutilscore_export.h"
#include <QString>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  class MDT_DEPLOYUTILSCORE_EXPORT DynamicSectionReadError : public QRuntimeError
  {
   public:

    /*! \brief Constructor
     */
    explicit DynamicSectionReadError(const QString & what)
      : QRuntimeError(what)
    {
    }
  };

  /*! \internal
   */
  class MDT_DEPLOYUTILSCORE_EXPORT NoteSectionReadError : public QRuntimeError
  {
   public:

    /*! \brief Constructor
     */
    explicit NoteSectionReadError(const QString & what)
      : QRuntimeError(what)
    {
    }
  };

  /*! \internal
   */
  class MDT_DEPLOYUTILSCORE_EXPORT MoveSectionError : public QRuntimeError
  {
   public:

    /*! \brief Constructor
     */
    explicit MoveSectionError(const QString & what)
      : QRuntimeError(what)
    {
    }
  };

  /*! \internal
   */
  class MDT_DEPLOYUTILSCORE_EXPORT GnuHashTableReadError : public QRuntimeError
  {
   public:

    /*! \brief Constructor
     */
    explicit GnuHashTableReadError(const QString & what)
      : QRuntimeError(what)
    {
    }
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_EXCEPTIONS_H
