/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2023 Philippe Steinmann.
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
#include "OperatingSystem.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QString>
#include <QFileInfo>
#include <memory>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

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
     *
     * \pre \a isExistingValidShLibOp must be a valid pointer
     */
    explicit AbstractSharedLibraryFinder(const std::shared_ptr<const AbstractIsExistingValidSharedLibrary> & isExistingValidShLibOp,
                                         QObject *parent = nullptr)
     : QObject(parent),
       mIsExistingValidShLibOp(isExistingValidShLibOp)
    {
      assert( mIsExistingValidShLibOp.get() != nullptr );
    }

    /*! \brief Returns the operating system this finder targets
     *
     * \post Returned OS is valid
     */
    OperatingSystem operatingSystem() const noexcept;

    /*! \brief Set a custom list of paths where this finder locates shared libraries
     */
    void setSearchPathList(const PathList & pathList) noexcept
    {
      mSearchPathList = pathList;
    }

    /*! \brief Get the list of paths where this finder locates shared libraries
     */
    const PathList & searchPathList() const noexcept
    {
      return mSearchPathList;
    }

    /*! \brief Check if given \a libraryName should be distributed
     *
     * \pre \a libraryName must not be empty
     */
    bool libraryShouldBeDistributed(const QString & libraryName) const noexcept;

    /*! \brief Find the absolute path for given \a libraryName
     *
     * \a dependentFile is the file that has \a libraryName as direct dependency.
     * This can typically be used to get rpath informations (for platforms that support it).
     *
     * \pre \a libraryName must not be empty
     * \pre given library should be distributed
     * \sa libraryShouldBeDistributed()
     * \exception FindDependencyError Thrown if given library could not be found
     * Other exception could be thrown (for example corrupted file have been read)
     */
    QFileInfo findLibraryAbsolutePath(const QString & libraryName, const BinaryDependenciesFile & dependentFile);

    /*! \brief Validate that given library is a existing shared library
     *
     * Will first use given validation operator (derived from AbstractIsExistingValidSharedLibrary),
     * then use validateSpecificSharedLibrary() .
     *
     * \pre \a libraryFile must be a absolute file path
     */
    bool validateIsExistingValidSharedLibrary(const QFileInfo & libraryFile);

   signals:

    void statusMessage(const QString & message) const;
    void verboseMessage(const QString & message) const;
    void debugMessage(const QString & message) const;

   private:

    /*! \brief Check if given library is valid reagarding library specific criteria
     *
     * Can be implemented if more checks have to be done to validate a specifc library.
     *
     * \pre \a libraryFile must be a absolute file path
     */
    virtual
    bool validateSpecificSharedLibrary(const QFileInfo & libraryFile);

    virtual
    OperatingSystem doOperatingSystem() const noexcept = 0;

    /*! \brief Check if given \a libraryName should be distributed
     *
     * This method has to be implemented by the concrete class.
     */
    virtual
    bool doLibraryShouldBeDistributed(const QString & libraryName) const noexcept = 0;

    /*! \brief Find the absolute path for given \a libraryName
     *
     * This method has to be implemented by the concrete class.
     *
     * The concrete implementation should search in various places
     * regarding the platform rules it targets.
     * For each file, it should call validateIsExistingValidSharedLibrary().
     * If the library could ne be found, a FindDependencyError should be thrown.
     *
     * \exception FindDependencyError Thrown if given library could not be found
     */
    virtual
    QFileInfo doFindLibraryAbsolutePath(const QString & libraryName, const BinaryDependenciesFile & dependentFile) = 0;

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
    //[[deprecated]]
    virtual
    BinaryDependenciesFile findLibraryAbsolutePath_OLD(const QString & libraryName,
                                                   const BinaryDependenciesFile & dependentFile) = 0;


    const std::shared_ptr<const AbstractIsExistingValidSharedLibrary> mIsExistingValidShLibOp;
    PathList mSearchPathList;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_ABSTRACT_SHARED_LIBRARY_FINDER_H
