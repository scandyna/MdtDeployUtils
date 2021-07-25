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
#include "LibraryExcludeListWindows.h"
#include "Mdt/DeployUtils/FindDependencyError.h"
#include "Mdt/DeployUtils/ExecutableFileReader.h"
#include "Mdt/DeployUtils/PathList.h"
#include "Mdt/DeployUtils/SearchPathList.h"
#include "Mdt/DeployUtils/Platform.h"
#include "mdt_deployutils_export.h"
#include <QString>
#include <QLatin1String>
#include <QStringList>
#include <QLatin1Char>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QObject>
#include <cassert>
#include <vector>
#include <algorithm>

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

      try{
        mReader.openFile(libraryFile,mPlatform);

        const bool isSharedLibrary = mReader.isExecutableOrSharedLibrary();

        const Platform libraryPlatform = mReader.getFilePlatform();
        const bool isCorrectProcessorISA = ( libraryPlatform.processorISA() == mPlatform.processorISA() );

        mReader.close();

        return isSharedLibrary && isCorrectProcessorISA;
      }catch(...){
        mReader.close();
        throw;
      }
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
      return libraryExcludelistLinux.contains(libraryName);
    };

    libraryNames.erase( std::remove_if( libraryNames.begin(), libraryNames.end(), pred), libraryNames.end() );
  }

  /*! \internal
   */
  inline
  void removeLibrariesInExcludeListWindows(QStringList & libraryNames) noexcept
  {
    const auto pred = [](const QString & libraryName){
      return libraryExcludelistWindows.contains(libraryName, Qt::CaseInsensitive);
    };

    libraryNames.erase( std::remove_if( libraryNames.begin(), libraryNames.end(), pred), libraryNames.end() );
  }

  /*! \internal
   */
  inline
  bool compareExecutableFileInfo(const ExecutableFileInfo & a, const ExecutableFileInfo & b) noexcept
  {
    const int directoryCmpValue = QString::compare(a.directoryPath, b.directoryPath);
    if(directoryCmpValue < 0){
      return true;
    }
    if(directoryCmpValue == 0){
      return (QString::compare(a.fileName, b.fileName) < 0);
    }

    return false;
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
  class MDT_DEPLOYUTILS_EXPORT FindDependenciesImpl : public QObject
  {
    Q_OBJECT

   public:

    FindDependenciesImpl(QObject *parent = nullptr)
     : QObject(parent)
    {
    }

    /*! \internal
     */
    template<typename Reader, typename IsExistingSharedLibraryOp>
    void findDependencies(const ExecutableFileInfo & currentFile, ExecutableFileInfoList & allDependencies, PathList searchPathList,
                          Reader & reader, const Platform & platform, IsExistingSharedLibraryOp & isExistingSharedLibraryOp)
    {
      assert( currentFile.hasAbsoluteFilePath() );
      assert( !reader.isOpen() );

      if( directDependenciesAreSolved(currentFile) ){
        return;
      }

      QFileInfo currentFileInfo = currentFile.toFileInfo();
      if( !currentFileInfo.exists() ){
        if( platform.operatingSystem() == OperatingSystem::Windows ){
          currentFileInfo = tryAlternativeFileNamesWindows(currentFileInfo);
        }
      }

      emitProcessingCurrentFileMessage(currentFileInfo);

      reader.openFile(currentFileInfo, platform);
      if( !reader.isExecutableOrSharedLibrary() ){
        const QString message = tr("'%1' is not a executable or a shared library")
                                .arg( currentFileInfo.absoluteFilePath() );
        throw FindDependencyError(message);
      }

      QStringList dependentLibraryNames = reader.getNeededSharedLibraries();

      emitDirectDependenciesMessage(currentFileInfo, dependentLibraryNames);

      if( platform.operatingSystem() == OperatingSystem::Linux ){
        removeLibrariesInExcludeListLinux(dependentLibraryNames);
      }
      if( platform.operatingSystem() == OperatingSystem::Windows ){
        removeLibrariesInExcludeListWindows(dependentLibraryNames);
      }

      const QStringList runPath = reader.getRunPath();
      reader.close();

      ExecutableFileInfoList dependencies = findLibrariesAbsolutePath(currentFile, dependentLibraryNames, runPath, searchPathList, platform, isExistingSharedLibraryOp);

      setDirectDependenciesSolved(currentFile);
      removeDuplicates(allDependencies);

      for(const ExecutableFileInfo & library : dependencies){
        allDependencies.push_back(library);
        findDependencies(library, allDependencies, searchPathList, reader, platform, isExistingSharedLibraryOp);
      }
    }

   signals:

    void verboseMessage(const QString & message) const;

   private:

    bool directDependenciesAreSolved(const ExecutableFileInfo & file) const noexcept
    {
      const auto pred = [&file](const ExecutableFileInfo & currentFile){
        return executableFileInfoAreEqual(currentFile, file);
      };

      return std::find_if(mSolvedFiles.cbegin(), mSolvedFiles.cend(), pred) != mSolvedFiles.cend();
    }

    void setDirectDependenciesSolved(const ExecutableFileInfo & file) noexcept
    {
      mSolvedFiles.push_back(file);
    }

    void emitProcessingCurrentFileMessage(const QFileInfo & file) const noexcept
    {
      const QString message = tr("searching dependencies for %1").arg( file.fileName() );
      emit verboseMessage(message);
    }

    void emitDirectDependenciesMessage(const QFileInfo & currentFile, const QStringList & dependencies) const noexcept
    {
      if( dependencies.isEmpty() ){
        return;
      }
      const QString startMessage = tr("%1 has following direct dependencies:").arg( currentFile.fileName() );
      emit verboseMessage(startMessage);
      for(const QString & dependency : dependencies){
        const QString msg = tr(" %1").arg(dependency);
        emit verboseMessage(msg);
      }
    }

    ExecutableFileInfoList mSolvedFiles;
  };

}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_H
