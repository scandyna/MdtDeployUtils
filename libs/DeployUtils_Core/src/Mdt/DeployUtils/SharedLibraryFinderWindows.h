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
#ifndef MDT_DEPLOY_UTILS_SHARED_LIBRARY_FINDER_WINDOWS_H
#define MDT_DEPLOY_UTILS_SHARED_LIBRARY_FINDER_WINDOWS_H

#include "BinaryDependenciesFile.h"
#include "PathList.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Helper to find a shared library on Windows
   */
  class MDT_DEPLOYUTILSCORE_EXPORT SharedLibraryFinderWindows : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit SharedLibraryFinderWindows(QObject *parent = nullptr);

    /*! \brief
     *
     * \sa https://docs.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-search-order
     */
    QString findLibraryAbsolutePath();

    
    
    /*! \brief Find the absolute path for each direct dependency of \a file
     */
    template<typename IsExistingSharedLibraryOp>
    static
    BinaryDependenciesFileList findLibrariesAbsolutePath(BinaryDependenciesFile & file,
                                                         const PathList & searchPathList,
                                                         IsExistingSharedLibraryOp & isExistingSharedLibraryOp)
    {
      /// \todo remove libraries in exclude list
    }

  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_SHARED_LIBRARY_FINDER_WINDOWS_H
