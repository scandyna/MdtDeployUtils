/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2022 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_SHARED_LIBRARY_FINDER_LINUX_H
#define MDT_DEPLOY_UTILS_SHARED_LIBRARY_FINDER_LINUX_H

#include "AbstractSharedLibraryFinder.h"
#include "BinaryDependenciesFile.h"
#include "FindDependencyError.h"
#include "PathList.h"
#include "RPath.h"
#include "ProcessorISA.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Helper to find a shared library on Linux
   */
  class MDT_DEPLOYUTILSCORE_EXPORT SharedLibraryFinderLinux : public AbstractSharedLibraryFinder
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit SharedLibraryFinderLinux(const AbstractIsExistingValidSharedLibrary & isExistingValidShLibOp, QObject *parent = nullptr) noexcept;

    /*! \brief Build a list of path to directories where to find shared libraries
     *
     * \sa PathList::getSystemLibraryKnownPathListLinux()
     * \sa https://man7.org/linux/man-pages/man8/ld.so.8.html
     */
    void buildSearchPathList(const PathList & searchFirstPathPrefixList, ProcessorISA processorISA) noexcept;

    /*! \internal
     */
    static
    QString makeDirectoryFromRpathEntry(const BinaryDependenciesFile & originFile, const RPathEntry & rpathEntry) noexcept;

    /*! \internal
     */
    BinaryDependenciesFile findLibraryAbsolutePathByRPath(const QString & libraryName,
                                                          const BinaryDependenciesFile & dependentFile) const;

    /*! \brief Find the absolute path for given \a libraryName
     *
     * \pre \a libraryName must not be empty
     * \exception FindDependencyError
     * \sa https://man7.org/linux/man-pages/man8/ld.so.8.html
     *
     * \todo case of \a libraryName containing slashes is not implemented
     * \todo LD_LIBRARY_PATH is not implemented
     */
    BinaryDependenciesFile findLibraryAbsolutePath(const QString & libraryName,
                                                   const BinaryDependenciesFile & dependentFile) const override;

   private:

    BinaryDependenciesFile findLibraryAbsolutePathBySearchPath(const QString & libraryName) const;

    /*! \brief Remove libraries that should not be distributed
     */
    void removeLibrariesToNotRedistribute(BinaryDependenciesFile & file) const noexcept override;

    static
    void removeLibrariesInLocalExcludeList(BinaryDependenciesFile & file) noexcept;

    static
    void removeLibrariesInGeneratedExcludeList(BinaryDependenciesFile & file) noexcept;
  };


}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_SHARED_LIBRARY_FINDER_LINUX_H
