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
#ifndef MDT_DEPLOY_UTILS_ABSTRACT_SHARED_LIBRARY_FINDER_H
#define MDT_DEPLOY_UTILS_ABSTRACT_SHARED_LIBRARY_FINDER_H

#include "BinaryDependenciesFile.h"
#include "PathList.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QFileInfo>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

  namespace Impl{
    class AbstractIsExistingSharedLibrary;
  } // namespace Impl{

  class AbstractIsExistingValidSharedLibrary;

  /*! \brief Interface to implement a shared library finder
   *
   * A concrete implementation should implement the required methods.
   * It should also provide a way to define a list of paths where to find the shared libraries,
   * and call setSearchPathList() .
   */
  class MDT_DEPLOYUTILSCORE_EXPORT AbstractSharedLibraryFinder : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit AbstractSharedLibraryFinder(const AbstractIsExistingValidSharedLibrary & isExistingValidShLibOp, QObject *parent = nullptr)
     : QObject(parent),
       mIsExistingValidShLibOp(isExistingValidShLibOp)
    {
    }

    /*! \brief Set a custom list of paths where this finders locates shared libraries
     */
    void setSearchPathList(const PathList & pathList) noexcept
    {
      mSearchPathList = pathList;
    }

    /*! \brief Get the list of paths where this finders locates shared libraries
     */
    const PathList & searchPathList() const noexcept
    {
      return mSearchPathList;
    }

    /*! \brief Find the absolute path for each direct dependency of \a file
     */
    BinaryDependenciesFileList findLibrariesAbsolutePath(BinaryDependenciesFile & file) const
    {
      return doFindLibrariesAbsolutePath(file);
    }

    /*! \brief Check if \a libraryFile is a existing shared library
     *
     * \pre \a libraryFile must be a absolute file path
     */
    bool isExistingValidSharedLibrary(const QFileInfo & libraryFile) const;

   private:

    virtual BinaryDependenciesFileList doFindLibrariesAbsolutePath(BinaryDependenciesFile & file) const = 0;

    const AbstractIsExistingValidSharedLibrary & mIsExistingValidShLibOp;
    PathList mSearchPathList;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_ABSTRACT_SHARED_LIBRARY_FINDER_H
