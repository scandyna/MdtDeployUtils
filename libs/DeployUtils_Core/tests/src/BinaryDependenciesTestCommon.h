/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2022 Philippe Steinmann.
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
#include "TestIsExistingSharedLibrary.h"
#include "Mdt/DeployUtils/Impl/BinaryDependencies.h"
#include "Mdt/DeployUtils/BinaryDependenciesFile.h"
#include "Mdt/DeployUtils/AbstractSharedLibraryFinder.h"
#include "Mdt/DeployUtils/PathList.h"
#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QMap>
#include <vector>
#include <string>

using namespace Mdt::DeployUtils;


inline
BinaryDependenciesFile makeBinaryDependenciesFileFromUtf8Path(const std::string & path)
{
  return BinaryDependenciesFile::fromQFileInfo( QString::fromStdString(path) );
}

inline
PathList makePathListFromUtf8Paths(const std::vector<std::string> & paths)
{
  PathList pathList;

  for(const std::string & path : paths){
    pathList.appendPath( QString::fromStdString(path) );
  }

  return pathList;
}

/*
 * Make a absolute path that retruns the correct result
 * on Linux and also on Windows.
 * As example: /tmp/file.txt will become C:/tmp/file.txt on Windows
 * See https://gitlab.com/scandyna/mdtdeployutils/-/jobs/1934576156
 */
inline
QString makeAbsolutePath(const std::string & path)
{
  return QFileInfo( QString::fromStdString(path) ).absoluteFilePath();
}


class SharedLibraryFinderBDTest : public AbstractSharedLibraryFinder
{
 public:

  explicit SharedLibraryFinderBDTest(const AbstractIsExistingValidSharedLibrary & isExistingValidShLibOp, QObject *parent = nullptr)
   : AbstractSharedLibraryFinder(isExistingValidShLibOp, parent)
  {
  }

  void setSearchPathList(const std::vector<std::string> & pathList)
  {
    for(const std::string & path : pathList){
      mSearchPathList.appendPath( QString::fromStdString(path) );
    }
  }

 private:

  BinaryDependenciesFile findLibraryAbsolutePath(const QString & libraryName) const
  {
    assert( !libraryName.trimmed().isEmpty() );

    for(const QString & directory : mSearchPathList){
      QFileInfo libraryFile(directory, libraryName);
      if( isExistingValidSharedLibrary(libraryFile) ){
        return BinaryDependenciesFile::fromQFileInfo(libraryFile);
      }
    }

    return BinaryDependenciesFile();
  }

  BinaryDependenciesFileList doFindLibrariesAbsolutePath(BinaryDependenciesFile & file) const override
  {
    BinaryDependenciesFileList libraries;

    for( const QString & libraryName : file.dependenciesFileNames() ){
      libraries.push_back( findLibraryAbsolutePath(libraryName) );
    }

    return libraries;
  }

  PathList mSearchPathList;
};
