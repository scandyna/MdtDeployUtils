/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2021 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_H
#define MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_H

#include "FindDependencyError.h"
#include "PathList.h"
#include "mdt_deployutils_export.h"
#include <QObject>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <memory>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Find dependencies for a executable or a library
   */
  class MDT_DEPLOYUTILS_EXPORT BinaryDependencies : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    BinaryDependencies(QObject* parent = nullptr);

    /*! \brief Find dependencies for a executable or a shared library
     *
     * At first, the target platform will be determined by \a binaryFilePath .
     *
     * For target platforms that support RPATH, they will be used for libraries that have them set.
     * Then, depending on target platform, dependencies will be searched in the directory of \a binaryFilePath .
     * Then, dependencies will be searched, for each path in \a searchFirstPathPrefixList ,
     *  in known subdirectories (like lib, bin, qt5/lib, qt5/bin).
     * Finally, depending on target platform, dependencies can also be serached in
     *  in system paths.
     *
     * Some libraries, known not have to be distributed,
     * will not be part of the result.
     *
     * Each library returned in the result will have its absolute file path.
     *
     * \pre \a binaryFilePath must have its path set
     * \exception FindDependencyError
     */
    QStringList findDependencies(const QFileInfo & binaryFilePath, const PathList & searchFirstPathPrefixList);

  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_H
