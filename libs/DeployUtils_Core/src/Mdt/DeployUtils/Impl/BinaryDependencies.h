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
#ifndef MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_H
#define MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_H

#include "LibraryExcludeListLinux.h"
#include "LibraryExcludeListWindows.h"

#include "Mdt/DeployUtils/BinaryDependenciesFile.h"
#include "Mdt/DeployUtils/FindDependencyError.h"
#include "Mdt/DeployUtils/ExecutableFileReader.h"
#include "Mdt/DeployUtils/PathList.h"
#include "Mdt/DeployUtils/SearchPathList.h"
#include "Mdt/DeployUtils/RPath.h"
#include "Mdt/DeployUtils/RPathElf.h"
#include "Mdt/DeployUtils/Platform.h"
#include "Mdt/DeployUtils/SharedLibraryFinderLinux.h"
#include "Mdt/DeployUtils/SharedLibraryFinderWindows.h"
#include "mdt_deployutilscore_export.h"
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

// #include <QDebug>

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
  [[deprecated]]
  inline
  QString makeDirectoryFromRpathEntry(const ExecutableFileInfo & originExecutable, const RPathEntry & rpathEntry) noexcept
  {
    assert( originExecutable.hasAbsoluteFilePath() );

    if( rpathEntry.isRelative() ){
      return QDir::cleanPath( originExecutable.directoryPath + QLatin1Char('/') + rpathEntry.path() );
    }

    return rpathEntry.path();
  }

  /*! \internal
   */
  template<typename IsExistingSharedLibraryOp>
  ExecutableFileInfo findLibraryAbsolutePathByRpath(const ExecutableFileInfo & originExecutable,
                                                    const QString & libraryName, const RPath & rpath,
                                                    IsExistingSharedLibraryOp & isExistingSharedLibraryOp)
  {
    assert( originExecutable.hasAbsoluteFilePath() );
    assert( !libraryName.trimmed().isEmpty() );
    assert( !rpath.isEmpty() );

    ExecutableFileInfo fi;

    for(const auto & rpathEntry : rpath){
      const QString directory = makeDirectoryFromRpathEntry(originExecutable, rpathEntry);
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
  ExecutableFileInfoList findLibrariesAbsolutePath_OLD(const ExecutableFileInfo & originExecutable, const QStringList & librariesNames,
                                                   const RPath & rpath, const PathList & searchPathList,
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
  template<typename IsExistingSharedLibraryOp>
  BinaryDependenciesFileList findLibrariesAbsolutePath(BinaryDependenciesFile & originExecutable,
                                                       const PathList & searchPathList,
                                                       const Platform & platform,
                                                       IsExistingSharedLibraryOp & isExistingSharedLibraryOp)
  {
    assert( !platform.isNull() );

    switch( platform.operatingSystem() ){
      case OperatingSystem::Linux:
        return SharedLibraryFinderLinux::findLibrariesAbsolutePath(originExecutable, searchPathList, isExistingSharedLibraryOp);
      case OperatingSystem::Windows:
        return SharedLibraryFinderWindows::findLibrariesAbsolutePath(originExecutable, searchPathList, isExistingSharedLibraryOp);
      case OperatingSystem::Unknown:
        break;
    }

    const QString message = QCoreApplication::translate("Mdt::DeployUtils::Impl::findLibrariesAbsolutePath()",
                                                        "requested platform is not supported");
    throw FindDependencyError(message);
  }

  /*! \internal
   */
  [[deprecated]]
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
  [[deprecated]]
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
  class MDT_DEPLOYUTILSCORE_EXPORT FindDependenciesImpl : public QObject
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
    void findDependencies(/*const*/ BinaryDependenciesFile & currentFile, ExecutableFileInfoList & allDependencies, PathList searchPathList,
                          Reader & reader, const Platform & platform, IsExistingSharedLibraryOp & isExistingSharedLibraryOp)
    {
      assert( !reader.isOpen() );

      ExecutableFileInfo currentFile_OLD;
      currentFile_OLD.fileName = currentFile.fileName();
      currentFile_OLD.directoryPath = currentFile.absoluteDirectoryPath();

      assert( currentFile_OLD.hasAbsoluteFilePath() );


      if( directDependenciesAreSolved(currentFile) ){
        return;
      }

      /** \todo
       *
       * The first call of this function IS with a existing file (see caller in BinaryDependencies)
       *
       * Then, if a dependency (on Windows)
       * is called LIBA.dll liba.dll, libA.DLL, etc...,
       * this must be fixed in the function finding the library.
       *
       * (info: this is usefull to solve for cross-deploy.
       *  on Windows, file names are case insensitive.
       *  If we deploy windows apps from a Linux machine,
       *  then we care about this)
       *
       * BUT: this block below should go away
       */
//       QFileInfo currentFileInfo = currentFile_OLD.toFileInfo();
//       if( !currentFileInfo.exists() ){
//         if( platform.operatingSystem() == OperatingSystem::Windows ){
//           currentFileInfo = tryAlternativeFileNamesWindows(currentFileInfo);
//         }
//       }

      emitProcessingCurrentFileMessage(currentFile);

      reader.openFile(currentFile.fileInfo(), platform);
      if( !reader.isExecutableOrSharedLibrary() ){
        const QString message = tr("'%1' is not a executable or a shared library")
                                .arg( currentFile.absoluteFilePath() );
        throw FindDependencyError(message);
      }

      currentFile.setDependenciesFileNames( reader.getNeededSharedLibraries() );

      /// \todo remove
      QStringList dependentLibraryNames = currentFile.dependenciesFileNames();

      emitDirectDependenciesMessage(currentFile);

      if( platform.operatingSystem() == OperatingSystem::Linux ){
        removeLibrariesInExcludeListLinux(dependentLibraryNames);
      }
      if( platform.operatingSystem() == OperatingSystem::Windows ){
        removeLibrariesInExcludeListWindows(dependentLibraryNames);
      }

      currentFile.setRPath( reader.getRunPath() );

      /// \todo remove
      const RPath runPath = currentFile.rPath();
      reader.close();

      ExecutableFileInfoList dependencies = findLibrariesAbsolutePath_OLD(currentFile_OLD, dependentLibraryNames, runPath, searchPathList, platform, isExistingSharedLibraryOp);

      setDirectDependenciesSolved(currentFile);
      removeDuplicates(allDependencies);

      for(const ExecutableFileInfo & library_OLD : dependencies){
        allDependencies.push_back(library_OLD);

        auto library = BinaryDependenciesFile::fromQFileInfo( library_OLD.toFileInfo() );
        findDependencies(library, allDependencies, searchPathList, reader, platform, isExistingSharedLibraryOp);
      }

      removeDuplicates(allDependencies);
    }

   signals:

    void verboseMessage(const QString & message) const;

   private:

    bool directDependenciesAreSolved(const BinaryDependenciesFile & file) const noexcept
    {
      const auto pred = [&file](const QFileInfo & currentFile){
        return file.absoluteFilePath() == currentFile.absoluteFilePath();
      };

      return std::find_if(mSolvedFiles.cbegin(), mSolvedFiles.cend(), pred) != mSolvedFiles.cend();
    }

//     bool directDependenciesAreSolved(const ExecutableFileInfo & file) const noexcept
//     {
//       const auto pred = [&file](const ExecutableFileInfo & currentFile){
//         return executableFileInfoAreEqual(currentFile, file);
//       };
//
//       return std::find_if(mSolvedFiles.cbegin(), mSolvedFiles.cend(), pred) != mSolvedFiles.cend();
//     }

    void setDirectDependenciesSolved(const BinaryDependenciesFile & file) noexcept
    {
      mSolvedFiles.push_back( file.fileInfo() );
    }

//     void setDirectDependenciesSolved(const ExecutableFileInfo & file) noexcept
//     {
//       mSolvedFiles.push_back(file);
//     }

    void emitProcessingCurrentFileMessage(const BinaryDependenciesFile & file) const noexcept
    {
      const QString message = tr("searching dependencies for %1").arg( file.fileName() );
      emit verboseMessage(message);
    }

    void emitDirectDependenciesMessage(const BinaryDependenciesFile & currentFile) const noexcept
    {
      if( currentFile.dependenciesFileNames().isEmpty() ){
        return;
      }
      const QString startMessage = tr("%1 has following direct dependencies:").arg( currentFile.fileName() );
      emit verboseMessage(startMessage);
      for( const QString & dependency : currentFile.dependenciesFileNames() ){
        const QString msg = tr(" %1").arg(dependency);
        emit verboseMessage(msg);
      }
    }

    std::vector<QFileInfo> mSolvedFiles;
//     ExecutableFileInfoList mSolvedFiles;
  };

}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_H
