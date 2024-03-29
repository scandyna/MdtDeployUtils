/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2020-2022 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_COPY_SHARED_LIBRARIES_TARGET_DEPENDS_ON_REQUEST_H
#define MDT_DEPLOY_UTILS_COPY_SHARED_LIBRARIES_TARGET_DEPENDS_ON_REQUEST_H

#include "OverwriteBehavior.h"
#include "CompilerLocationRequest.h"
#include "mdt_deployutilscore_export.h"
#include <QStringList>
#include <QString>

namespace Mdt{ namespace DeployUtils{

  /*! \brief DTO for CopySharedLibrariesTargetDependsOn
   */
  struct MDT_DEPLOYUTILSCORE_EXPORT CopySharedLibrariesTargetDependsOnRequest
  {
    OverwriteBehavior overwriteBehavior = OverwriteBehavior::Fail;
    bool removeRpath = false;
//     CompilerLocationType compilerLocationType = CompilerLocationType::Undefined;
//     QString compilerLocationValue;
    CompilerLocationRequest compilerLocation;
    QStringList searchPrefixPathList;
    QString targetFilePath;
    QString destinationDirectoryPath;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_COPY_SHARED_LIBRARIES_TARGET_DEPENDS_ON_REQUEST_H
