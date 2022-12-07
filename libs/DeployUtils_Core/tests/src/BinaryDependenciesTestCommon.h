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
#include "TestUtils.h"
#include "TestFileUtils.h"
#include "Mdt/DeployUtils/Impl/BinaryDependencies.h"
#include "Mdt/DeployUtils/BinaryDependenciesFile.h"

#include "Mdt/DeployUtils/BinaryDependenciesResult.h"

// #include "Mdt/DeployUtils/QtDistributionDirectory.h"

#include "Mdt/DeployUtils/AbstractSharedLibraryFinder.h"
#include "Mdt/DeployUtils/PathList.h"
#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QMap>
#include <vector>
#include <string>
#include <memory>

using namespace Mdt::DeployUtils;


bool containsLibrary(const BinaryDependenciesResult & result, const QString & libraryName)
{
  const auto getLibraryName = [](const BinaryDependenciesResultLibrary & library){
    return library.libraryName();
  };

  return containsLibrary(result, getLibraryName, libraryName);
}

bool containsQt5Core(const BinaryDependenciesResult & result)
{
  return containsLibrary(result, QLatin1String("Qt5Core"));
}

bool containsTestSharedLibrary(const BinaryDependenciesResult & result)
{
  return containsLibrary( result, QLatin1String("testSharedLibrary") );
}


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

  OperatingSystem doOperatingSystem() const noexcept override
  {
    return OperatingSystem::Linux;
  }

  bool doLibraryShouldBeDistributed(const QString & /*libraryName*/) const noexcept override
  {
    return true;
  }

  QFileInfo doFindLibraryAbsolutePath(const QString & libraryName, const BinaryDependenciesFile & /*dependentFile*/) const override
  {
    assert( !libraryName.trimmed().isEmpty() );

    for(const QString & directory : mSearchPathList){
      QFileInfo libraryFile(directory, libraryName);
      if( isExistingValidSharedLibrary(libraryFile) ){
        return libraryFile;
      }
    }

    const QString message = tr("could not find the absolute path for %1")
                            .arg(libraryName);
    throw FindDependencyError(message);
  }

  void removeLibrariesToNotRedistribute(BinaryDependenciesFile & /*file*/) const noexcept override
  {
  }

  BinaryDependenciesFile findLibraryAbsolutePath_OLD(const QString & libraryName,
                                                  const BinaryDependenciesFile & /*dependentFile*/) const override
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

  PathList mSearchPathList;
};
