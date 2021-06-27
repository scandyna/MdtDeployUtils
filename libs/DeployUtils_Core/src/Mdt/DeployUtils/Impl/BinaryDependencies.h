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
#ifndef MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_H
#define MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_H

#include "LibraryExcludeListLinux.h"
#include "Mdt/DeployUtils/FindDependencyError.h"
#include "Mdt/DeployUtils/ExecutableFileReader.h"
#include "Mdt/DeployUtils/PathList.h"
#include "Mdt/DeployUtils/SearchPathList.h"
#include "Mdt/DeployUtils/Platform.h"
#include <QString>
#include <QLatin1String>
#include <QStringList>
#include <QLatin1Char>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <cassert>
#include <vector>
#include <algorithm>

#include <QDebug>

namespace Mdt{ namespace DeployUtils{ namespace Impl{

  /*! \internal
   */
  struct ExecutableFileInfo
  {
    QString fileName;
    QString directoryPath;

    /*! \brief Check if this executable file is null
    */
    bool isNull() const noexcept
    {
      return fileName.isEmpty();
    }

    /*! \brief Check if this executable file has a absolute path
    */
    bool hasAbsoluteFilePath() const noexcept
    {
      if( isNull() ){
        return false;
      }
      if( directoryPath.isEmpty() ){
        return false;
      }

      return true;
    }

    QFileInfo toFileInfo() const
    {
      assert( hasAbsoluteFilePath() );

      return QFileInfo(directoryPath, fileName);
    }
  };

  using ExecutableFileInfoList = std::vector<ExecutableFileInfo>;

  /*! \internal
   */
  inline
  QStringList qStringListFromExecutableFileInfoList(const ExecutableFileInfoList & executables)
  {
    QStringList list;

    for(const auto & file : executables){
      list.push_back( file.toFileInfo().absoluteFilePath() );
    }

    return list;
  }

  /*! \internal
   */
  inline
  QFileInfo tryAlternativeFileNamesWindows(const QFileInfo & file) noexcept
  {
    QFileInfo alternativeFile = file;

    const QDir directory = file.absoluteDir();
    alternativeFile.setFile( directory, file.fileName().toUpper() );
    if( alternativeFile.exists() ){
      return alternativeFile;
    }
    alternativeFile.setFile( directory, file.fileName().toLower() );

    return alternativeFile;
  }

  /*! \internal
   */
  struct IsExistingSharedLibraryFunc
  {
    IsExistingSharedLibraryFunc(ExecutableFileReader & reader, const Platform & platform) noexcept
     : mReader(reader),
       mPlatform(platform)
    {
    }

    /*! \internal
     *
     * \pre \a libraryFile must be a absolute file path
     */
    bool operator()(const QFileInfo & libraryFile)
    {
      assert( !libraryFile.filePath().isEmpty() ); // see doc of QFileInfo::absoluteFilePath()
      assert( libraryFile.isAbsolute() );
      assert( !mReader.isOpen() );

      if( !libraryFile.exists() ){
        return false;
      }

      mReader.openFile(libraryFile,mPlatform);

      /// \todo should be isSharedLibrary()
      const bool isSharedLibrary = mReader.isExecutableOrSharedLibrary();

      const Platform libraryPlatform = mReader.getFilePlatform();
      const bool isCorrectProcessorISA = ( libraryPlatform.processorISA() == mPlatform.processorISA() );

      mReader.close();

      return isSharedLibrary && isCorrectProcessorISA;
    }

   private:

    ExecutableFileReader & mReader;
    Platform mPlatform;
  };

  /*! \internal
   */
  inline
  QString makeDirectoryFromRpath(const ExecutableFileInfo & originExecutable, const QString & rpathItem) noexcept
  {
    assert( originExecutable.hasAbsoluteFilePath() );
    assert( !rpathItem.trimmed().isEmpty() );

    if( rpathItem.startsWith( QLatin1String("$ORIGIN") ) ){
      return QDir::cleanPath( originExecutable.directoryPath + QLatin1Char('/') + rpathItem.right( rpathItem.length() - 7) );
    }

    return rpathItem;
  }

  /*! \internal
   */
  template<typename IsExistingSharedLibraryOp>
  ExecutableFileInfo findLibraryAbsolutePathByRpath(const ExecutableFileInfo & originExecutable,
                                                    const QString & libraryName, const QStringList & rpath,
                                                    IsExistingSharedLibraryOp & isExistingSharedLibraryOp)
  {
    assert( originExecutable.hasAbsoluteFilePath() );
    assert( !libraryName.trimmed().isEmpty() );
    assert( !rpath.isEmpty() );

    ExecutableFileInfo fi;

    for(const auto & rpathItem : rpath){
      const QString directory = makeDirectoryFromRpath(originExecutable, rpathItem);
      const QFileInfo libraryFile(directory, libraryName);
      if( isExistingSharedLibraryOp(libraryFile) ){
        fi.fileName = libraryName;
        fi.directoryPath = libraryFile.absoluteDir().path();
        return fi;
      }
    }

    return fi;
  }

  /*! \internal
   *
   * \pre \a libraryName must not be empty
   * \pre \a searchPathList must not be empty
   * \exception FindDependencyError
   */
  template<typename IsExistingSharedLibraryOp>
  ExecutableFileInfo findLibraryAbsolutePath(const QString & libraryName, const PathList & searchPathList,
                                             const Platform & platform,
                                             IsExistingSharedLibraryOp & isExistingSharedLibraryOp)
  {
    assert( !libraryName.trimmed().isEmpty() );
    assert( !searchPathList.isEmpty() );

    ExecutableFileInfo fi;

    for(const auto & directory : searchPathList){
      QFileInfo libraryFile(directory, libraryName);
      if( isExistingSharedLibraryOp(libraryFile) ){
        fi.fileName = libraryFile.fileName();
        fi.directoryPath = libraryFile.absoluteDir().path();
        return fi;
      }
      if( platform.operatingSystem() == OperatingSystem::Windows ){
        libraryFile = tryAlternativeFileNamesWindows(libraryFile);
        if( isExistingSharedLibraryOp(libraryFile) ){
          fi.fileName = libraryFile.fileName();
          fi.directoryPath = libraryFile.absoluteDir().path();
          return fi;
        }
      }
    }

    const QString message = QCoreApplication::translate("Mdt::DeployUtils::Impl::findLibraryAbsolutePath()",
                                                        "could not find the absolute path for %1")
                            .arg(libraryName);
    throw FindDependencyError(message);

//     return fi;
  }

  /*! \internal
   */
  template<typename IsExistingSharedLibraryOp>
  ExecutableFileInfoList findLibrariesAbsolutePath(const ExecutableFileInfo & originExecutable, const QStringList & librariesNames,
                                                   const QStringList & rpath, const PathList & searchPathList,
                                                   const Platform & platform,
                                                   IsExistingSharedLibraryOp & isExistingSharedLibraryOp)
  {
    assert( originExecutable.hasAbsoluteFilePath() );

    ExecutableFileInfoList libraries;

    for(const QString & libraryName : librariesNames){
      ExecutableFileInfo library;
      if( !rpath.isEmpty() ){
        library = findLibraryAbsolutePathByRpath(originExecutable, libraryName, rpath, isExistingSharedLibraryOp);
      }
      if( library.isNull() ){
        library =  findLibraryAbsolutePath(libraryName, searchPathList, platform, isExistingSharedLibraryOp);
      }
      libraries.push_back(library);
    }

    return libraries;
  }

  /*! \internal
   */
  inline
  void removeLibrariesInExcludeListLinux(QStringList & libraryNames) noexcept
  {
    const auto pred = [](const QString & libraryName){
      return generatedExcludelistLinux.contains(libraryName);
    };

    libraryNames.erase( std::remove_if( libraryNames.begin(), libraryNames.end(), pred), libraryNames.end() );
  }

  /*! \internal
   */
  inline
  bool compareExecutableFileInfo(const ExecutableFileInfo & a, const ExecutableFileInfo & b) noexcept
  {
    return (QString::compare(a.fileName, b.fileName) < 0) || (QString::compare(a.directoryPath, b.directoryPath) < 0);
  }

  /*! \internal
   */
  inline
  bool executableFileInfoAreEqual(const ExecutableFileInfo & a, const ExecutableFileInfo & b) noexcept
  {
    return (a.fileName == b.fileName) && (a.directoryPath == b.directoryPath);
  }

  /*! \internal
   */
  inline
  void removeDuplicates(ExecutableFileInfoList & executables) noexcept
  {
    std::sort(executables.begin(), executables.end(), compareExecutableFileInfo);
    executables.erase( std::unique(executables.begin(), executables.end(), executableFileInfoAreEqual), executables.end() );
  }

  /*! \internal
   */
  template<typename Reader, typename IsExistingSharedLibraryOp>
  void findDependencies(const ExecutableFileInfo & currentFile, ExecutableFileInfoList & allDependencies, PathList searchPathList,
                        Reader & reader, const Platform & platform, IsExistingSharedLibraryOp & isExistingSharedLibraryOp)
  {
    assert( currentFile.hasAbsoluteFilePath() );
    assert( !reader.isOpen() );

    qDebug() << "solve for " << currentFile.fileName;
    
    QFileInfo currentFileInfo = currentFile.toFileInfo();
    if( !currentFileInfo.exists() ){
      if( platform.operatingSystem() == OperatingSystem::Windows ){
        currentFileInfo = tryAlternativeFileNamesWindows(currentFileInfo);
      }
    }

    reader.openFile(currentFileInfo, platform);
    if( !reader.isExecutableOrSharedLibrary() ){
      qDebug() << " not a exe";
      /// \todo Exception
      return;
    }

    QStringList dependentLibraryNames = reader.getNeededSharedLibraries();

    if( platform.operatingSystem() == OperatingSystem::Linux ){
      removeLibrariesInExcludeListLinux(dependentLibraryNames);
    }
    if( platform.operatingSystem() == OperatingSystem::Windows ){
      /// \todo remove also for Windows
    }

    const QStringList runPath = reader.getRunPath();
    reader.close();

    ExecutableFileInfoList dependencies = findLibrariesAbsolutePath(currentFile, dependentLibraryNames, runPath, searchPathList, platform, isExistingSharedLibraryOp);

    removeDuplicates(allDependencies);

    for(const ExecutableFileInfo & library : dependencies){
      allDependencies.push_back(library);
      findDependencies(library, allDependencies, searchPathList, reader, platform, isExistingSharedLibraryOp);
    }
  }


  /*! \internal Build a list of search path
   */
  inline
  PathList buildSearchPathList(const QFileInfo & binaryFilePath, const PathList & searchFirstPathPrefixList, const Platform & platform) noexcept
  {
    assert( !binaryFilePath.path().isEmpty() );

    PathList pathList;

    /// \todo should check which path to add depending on cpu (32/64bit)
    SearchPathList searchFirstPathList;
    searchFirstPathList.setIncludePathPrefixes(true);
    if( platform.operatingSystem() == OperatingSystem::Windows ){
      searchFirstPathList.setPathSuffixList({QLatin1String("bin"),QLatin1String("qt5/bin")});
      searchFirstPathList.appendPath( binaryFilePath.absoluteDir().path() );
    }else{
      searchFirstPathList.setPathSuffixList({QLatin1String("lib"),QLatin1String("qt5/lib")});
    }
    searchFirstPathList.setPathPrefixList(searchFirstPathPrefixList);

    pathList.appendPathList( searchFirstPathList.pathList() );
    pathList.appendPathList( PathList::getSystemLibraryKnownPathList(platform) );

    return pathList;
  }

}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_H
