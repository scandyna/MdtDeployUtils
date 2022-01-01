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
#include <cassert>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Helper to find a shared library on Linux
   */
  class MDT_DEPLOYUTILSCORE_EXPORT SharedLibraryFinderLinux : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit SharedLibraryFinderLinux(QObject *parent = nullptr);

    /*! \brief Build and returns a list of path to directories where to find shared libraries
     *
     * \sa PathList::getSystemLibraryKnownPathListLinux()
     * \sa https://man7.org/linux/man-pages/man8/ld.so.8.html
     */
    static
    PathList buildSearchPathList(const PathList & searchFirstPathPrefixList, ProcessorISA processorISA) noexcept;

    /*! \brief Find the absolute path for given \a libraryName
     *
     * \pre \a libraryName must not be empty
     * \exception FindDependencyError
     * \sa https://man7.org/linux/man-pages/man8/ld.so.8.html
     *
     * \todo case of \a libraryName containing slashes is not implemented
     * \todo LD_LIBRARY_PATH is not implemented
     */
    template<typename IsExistingSharedLibraryOp>
    static
    BinaryDependenciesFile findLibraryAbsolutePath(const BinaryDependenciesFile & originFile,
                                                   const QString & libraryName,
                                                   const PathList & searchPathList,
                                                   IsExistingSharedLibraryOp & isExistingSharedLibraryOp)
    {
      assert( !originFile.isNull() );
      assert( !libraryName.trimmed().isEmpty() );

      BinaryDependenciesFile library = findLibraryAbsolutePathByRPath(originFile, libraryName, isExistingSharedLibraryOp);

      if( library.isNull() ){
        library = findLibraryAbsolutePathBySearchPath(libraryName, searchPathList, isExistingSharedLibraryOp);
      }

      if( library.isNull() ){
        const QString message = tr("could not find the absolute path for %1")
                                .arg(libraryName);
        throw FindDependencyError(message);
      }

      return library;
    }

    /*! \brief Find the absolute path for each direct dependency of \a file
     */
    template<typename IsExistingSharedLibraryOp>
    static
    BinaryDependenciesFileList findLibrariesAbsolutePath(BinaryDependenciesFile & file,
                                                         const PathList & searchPathList,
                                                         IsExistingSharedLibraryOp & isExistingSharedLibraryOp)
    {
      BinaryDependenciesFileList libraries;

      removeLibrariesInExcludeList(file);

      for( const QString & libraryName : file.dependenciesFileNames() ){
        libraries.push_back( findLibraryAbsolutePath(file, libraryName, searchPathList, isExistingSharedLibraryOp) );
      }

      return libraries;
    }

    /*! \internal
     */
    static
    QString makeDirectoryFromRpathEntry(const BinaryDependenciesFile & originFile, const RPathEntry & rpathEntry) noexcept;

   private:

    template<typename IsExistingSharedLibraryOp>
    static
    BinaryDependenciesFile findLibraryAbsolutePathByRPath(const BinaryDependenciesFile & originFile,
                                                          const QString & libraryName,
                                                          IsExistingSharedLibraryOp & isExistingSharedLibraryOp)
    {
      assert( !libraryName.trimmed().isEmpty() );

      for( const auto & rpathEntry : originFile.rPath() ){
        const QString directory = makeDirectoryFromRpathEntry(originFile, rpathEntry);
        const QFileInfo libraryFile(directory, libraryName);
        if( isExistingSharedLibraryOp(libraryFile) ){
          return BinaryDependenciesFile::fromQFileInfo(libraryFile);
        }
      }

      return BinaryDependenciesFile();
    }

    template<typename IsExistingSharedLibraryOp>
    static
    BinaryDependenciesFile findLibraryAbsolutePathBySearchPath(const QString & libraryName,
                                                               const PathList & searchPathList,
                                                               IsExistingSharedLibraryOp & isExistingSharedLibraryOp)
    {
      assert( !libraryName.trimmed().isEmpty() );

      for(const QString & directory : searchPathList){
        QFileInfo libraryFile(directory, libraryName);
        if( isExistingSharedLibraryOp(libraryFile) ){
          return BinaryDependenciesFile::fromQFileInfo(libraryFile);
        }
      }

      return BinaryDependenciesFile();
    }

    static
    void removeLibrariesInExcludeList(BinaryDependenciesFile & file) noexcept;
  };


}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_SHARED_LIBRARY_FINDER_LINUX_H
