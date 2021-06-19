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
#ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_IMPLEMENTATION_INTERFACE_H
#define MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_IMPLEMENTATION_INTERFACE_H

#include "LibraryInfoList.h"
#include "PathList.h"
#include "mdt_deployutils_export.h"
#include <QString>
#include <QStringList>
#include <QObject>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Interface to implement a backend for BinaryDependencies
   */
  class MDT_DEPLOYUTILS_EXPORT BinaryDependenciesImplementationInterface : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit BinaryDependenciesImplementationInterface(QObject* parent = nullptr);

    /*! \brief Set a list of paths where to search dependencies first
     */
    void setLibrarySearchFirstPathList(const Mdt::FileSystem::PathList & pathList);

    /*! \brief Get the list of paths where to search dependencies first
     */
    Mdt::FileSystem::PathList librarySearchFirstPathList() const
    {
      return mLibrarySearchFirstPathList;
    }

    /*! \brief Find dependencies for a executable or a library
     */
    virtual bool findDependencies(const QString & binaryFilePath) = 0;

    /*! \brief Find dependencies for a lits of binaries
     *
     * This default implementation simply calls findDependencies(const QString &)
     *  for each binary file in \a binariesFilePaths .
     *
     * \pre \a binariesFilePaths must not be a empty list
     */
    virtual bool findDependencies(const QStringList & binariesFilePaths);

    /*! \brief Find dependencies for a lits of libraries
     *
     * This default implementation simply calls findDependencies(const QString &)
     *  for each library in \a libraries .
     *
     * \pre \a libraries must not be a empty list
     */
    virtual bool findDependencies(const LibraryInfoList & libraries);

    /*! \brief Get dependencies
     */
    LibraryInfoList dependencies() const
    {
      return mDependencies;
    }

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   protected:

    /*! \brief Set dependencies
     */
    void setDependencies(const LibraryInfoList & dependencies);

    /*! \brief Set dependencies
     */
    void setDependencies(const LibraryInfoList & dependencies, const QStringList & librariesToExclude);

    /*! \brief Set dependencies
     */
    void setDependencies(const LibraryInfoList & dependencies, const LibraryInfoList & librariesToExclude);

    /*! \brief Set last error
     */
    void setLastError(const Mdt::Error & error);

   private:

    static bool listContainsLibrary(const QStringList & list, const LibraryInfo & li);
    static bool listContainsLibrary(const LibraryInfoList & list, const LibraryInfo & li);

    LibraryInfoList mDependencies;
    PathList mLibrarySearchFirstPathList;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_IMPLEMENTATION_INTERFACE_H
