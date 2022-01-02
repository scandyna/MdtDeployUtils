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
  inline
  QStringList qStringListFromBinaryDependenciesFileList(const BinaryDependenciesFileList & files) noexcept
  {
    QStringList list;

    for(const auto & file : files){
      list.push_back( file.absoluteFilePath() );
    }

    return list;
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
  inline
  bool compareBinaryDependenciesFiles(const BinaryDependenciesFile & a, const BinaryDependenciesFile & b) noexcept
  {
    return (QString::compare( a.absoluteFilePath(), b.absoluteFilePath() ) < 0);
  }

  /*! \internal
   */
  inline
  bool binaryDependenciesFilesAreEqual(const BinaryDependenciesFile & a, const BinaryDependenciesFile & b) noexcept
  {
    return a.absoluteFilePath() == b.absoluteFilePath();
  }

  /*! \internal
   */
  inline
  void removeDuplicates(BinaryDependenciesFileList & files) noexcept
  {
    std::sort(files.begin(), files.end(), compareBinaryDependenciesFiles);
    files.erase( std::unique(files.begin(), files.end(), binaryDependenciesFilesAreEqual), files.end() );
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
    void findDependencies(BinaryDependenciesFile & currentFile, BinaryDependenciesFileList & allDependencies, PathList searchPathList,
                          Reader & reader, const Platform & platform, IsExistingSharedLibraryOp & isExistingSharedLibraryOp)
    {
      assert( !reader.isOpen() );

      if( directDependenciesAreSolved(currentFile) ){
        return;
      }

      emitProcessingCurrentFileMessage(currentFile);

      reader.openFile(currentFile.fileInfo(), platform);
      if( !reader.isExecutableOrSharedLibrary() ){
        const QString message = tr("'%1' is not a executable or a shared library")
                                .arg( currentFile.absoluteFilePath() );
        throw FindDependencyError(message);
      }

      currentFile.setDependenciesFileNames( reader.getNeededSharedLibraries() );

      emitDirectDependenciesMessage(currentFile);

      currentFile.setRPath( reader.getRunPath() );

      reader.close();

      BinaryDependenciesFileList dependencies = findLibrariesAbsolutePath(currentFile, searchPathList, platform, isExistingSharedLibraryOp);

      setDirectDependenciesSolved(currentFile);
      removeDuplicates(allDependencies);

      for(BinaryDependenciesFile & library : dependencies){
        allDependencies.push_back(library);
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

    void setDirectDependenciesSolved(const BinaryDependenciesFile & file) noexcept
    {
      mSolvedFiles.push_back( file.fileInfo() );
    }

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
  };

}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_H
