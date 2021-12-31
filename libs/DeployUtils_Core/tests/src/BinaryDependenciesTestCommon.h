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
#include "Mdt/DeployUtils/Impl/BinaryDependencies.h"
#include "Mdt/DeployUtils/BinaryDependenciesFile.h"
#include "Mdt/DeployUtils/PathList.h"
#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QMap>
#include <vector>
#include <string>

using namespace Mdt::DeployUtils;
using Impl::ExecutableFileInfo;
using Impl::ExecutableFileInfoList;


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


class TestIsExistingSharedLibrary
{
 public:

  void setExistingSharedLibraries(const std::vector<std::string> & libraries)
  {
    for(const auto & library : libraries){
      const QFileInfo libraryFile( QString::fromStdString(library) );
      assert( libraryFile.isAbsolute() );
      mExistingSharedLibraries.append( libraryFile.absoluteFilePath() );
    }
  }

  /*! \internal
   *
   * \pre \a libraryFile must be a absolute file path
   */
  bool operator()(const QFileInfo & libraryFile) const
  {
    assert( !libraryFile.filePath().isEmpty() ); // see doc of QFileInfo::absoluteFilePath()
    assert( libraryFile.isAbsolute() );

    return mExistingSharedLibraries.contains( libraryFile.absoluteFilePath() );
  }

 private:

  QStringList mExistingSharedLibraries;
};
