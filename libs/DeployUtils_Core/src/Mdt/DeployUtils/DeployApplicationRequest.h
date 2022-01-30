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
#ifndef MDT_DEPLOY_UTILS_DEPLOY_APPLICATION_REQUEST_H
#define MDT_DEPLOY_UTILS_DEPLOY_APPLICATION_REQUEST_H

#include "OverwriteBehavior.h"
#include "CompilerLocationRequest.h"
#include "mdt_deployutilscore_export.h"
#include <QString>
#include <QStringList>

namespace Mdt{ namespace DeployUtils{

  /*! \brief DTO for DeployApplication
   */
  struct MDT_DEPLOYUTILSCORE_EXPORT DeployApplicationRequest
  {
    QString targetFilePath;
    QString destinationDirectoryPath;
    QStringList searchPrefixPathList;
    CompilerLocationRequest compilerLocation;
    OverwriteBehavior shLibOverwriteBehavior = OverwriteBehavior::Fail;
    bool removeRpath = false;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_DEPLOY_APPLICATION_REQUEST_H
