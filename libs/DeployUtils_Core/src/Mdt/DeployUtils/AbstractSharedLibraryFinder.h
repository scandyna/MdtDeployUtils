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
#include "RPath.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QString>
#include <QFileInfo>
#include <memory>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

//   namespace Impl{
//     class AbstractIsExistingSharedLibrary;
//   } // namespace Impl{

  class AbstractIsExistingValidSharedLibrary;
  class QtDistributionDirectory;

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
     *
     * If given \a qtDistributionDirectory is not already initialzed,
     * it will be the first time a Qt library have been found.
     *
     * \pre \a qtDistributionDirectory must be a valid pointer
     */
    explicit AbstractSharedLibraryFinder(const AbstractIsExistingValidSharedLibrary & isExistingValidShLibOp,
                                         std::shared_ptr<QtDistributionDirectory> qtDistributionDirectory,
                                         QObject *parent = nullptr)
     : QObject(parent),
       mIsExistingValidShLibOp(isExistingValidShLibOp)
    {
      assert(qtDistributionDirectory.get() != nullptr);
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
    BinaryDependenciesFileList findLibrariesAbsolutePath(BinaryDependenciesFile & file); /*const*/

    /*! \brief Check if \a libraryFile is a existing shared library
     *
     * \pre \a libraryFile must be a absolute file path
     */
    bool isExistingValidSharedLibrary(const QFileInfo & libraryFile) const;

   signals:

    void statusMessage(const QString & message) const;
    void verboseMessage(const QString & message) const;
    void debugMessage(const QString & message) const;

   private:

    /*! \brief Remove libraries that should not be distributed
     *
     * This method has to be implemented by the concrete class.
     */
    virtual void removeLibrariesToNotRedistribute(BinaryDependenciesFile & file) const noexcept = 0;

    /*! \brief Find the absolute path for given \a libraryName
     *
     * This method has to be implemented by the concrete class.
     *
     * \a dependentFile if the file that has \a libraryName as direct dependency.
     * This can typically be used to get rpath informations (for platforms that support it).
     *
     * \pre \a libraryName must not be empty
     * \exception FindDependencyError
     */
    virtual
    BinaryDependenciesFile findLibraryAbsolutePath(const QString & libraryName,
                                                   const BinaryDependenciesFile & dependentFile) const = 0;

    const AbstractIsExistingValidSharedLibrary & mIsExistingValidShLibOp;
    PathList mSearchPathList;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_ABSTRACT_SHARED_LIBRARY_FINDER_H
