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
#include "Mdt/DeployUtils/AbstractSharedLibraryFinder.h"
#include "Mdt/DeployUtils/Platform.h"
#include "Mdt/DeployUtils/Algorithm.h"
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
#include <memory>

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

    /*! \brief Constructor
     */
    explicit FindDependenciesImpl(QObject *parent = nullptr)
     : QObject(parent)
    {
    }

    /*! \internal
     */
    void setSharedLibrariesFinder(const std::shared_ptr<AbstractSharedLibraryFinder> & shLibFinder) noexcept
    {
      assert( shLibFinder.get() != nullptr );

      mShLibFinder = shLibFinder;
    }

    /*! \internal
     */
    template<typename Reader>
    void findDependencies(BinaryDependenciesFile & currentFile, BinaryDependenciesFileList & allDependencies,
                          Reader & reader, const Platform & platform)
    {
      assert( !reader.isOpen() );
      assert( mShLibFinder.get() != nullptr );

      if( directDependenciesAreSolved(currentFile) ){
        return;
      }
      emitAlreadySolvedFilesMessage();

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

      BinaryDependenciesFileList dependencies = mShLibFinder->findLibrariesAbsolutePath(currentFile);

      emitSolvedDirectDependenciesMessage(currentFile, dependencies);

      setDirectDependenciesSolved(currentFile);
      removeDuplicates(allDependencies);

      for(BinaryDependenciesFile & library : dependencies){
        allDependencies.push_back(library);
        findDependencies(library, allDependencies, reader, platform);
      }

      removeDuplicates(allDependencies);
    }

   signals:

    void verboseMessage(const QString & message) const;
    void debugMessage(const QString & message) const;

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

    void emitAlreadySolvedFilesMessage()
    {
      if( mSolvedFiles.empty() ){
        return;
      }

      const auto toQString = [](const QFileInfo & fi){
        return fi.fileName();
      };

      const QString solvedFilesString = joinToQString( mSolvedFiles, toQString, QLatin1String(", ") );

      emit debugMessage(
        tr("already solved dependencies: %1").arg(solvedFilesString)
      );
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

    void emitSolvedDirectDependenciesMessage(const BinaryDependenciesFile & currentFile, const BinaryDependenciesFileList & dependencies) const noexcept
    {
      if( dependencies.empty() ){
        return;
      }
      emit debugMessage(
        tr("solved direct dependencies for %1:")
        .arg( currentFile.fileName() )
      );
      for(const auto & dependency : dependencies){
        emit debugMessage(
          tr(" %1")
          .arg( dependency.absoluteFilePath() )
        );
      }
    }

    std::shared_ptr<AbstractSharedLibraryFinder> mShLibFinder;
    std::vector<QFileInfo> mSolvedFiles;
  };

}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_H
