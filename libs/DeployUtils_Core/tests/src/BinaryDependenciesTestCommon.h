/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2023 Philippe Steinmann.
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
#include "Mdt/DeployUtils/FindDependencyError.h"
#include "Mdt/DeployUtils/BinaryDependenciesFile.h"
#include "Mdt/DeployUtils/BinaryDependenciesResultLibrary.h"
#include "Mdt/DeployUtils/BinaryDependenciesResult.h"
#include "Mdt/DeployUtils/AbstractSharedLibraryFinder.h"
#include "Mdt/DeployUtils/PathList.h"
#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QMap>
#include <vector>
#include <algorithm>
#include <string>
#include <memory>
#include <cassert>

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
bool libraryListContainsPath(const std::vector<BinaryDependenciesResultLibrary> & libraryList, const std::string & path)
{
  const QString qPath = makeAbsolutePath(path);

  const auto pred = [&qPath](const BinaryDependenciesResultLibrary & library){
    assert( library.isFound() );
    return library.absoluteFilePath() == qPath;
  };

  const auto it = std::find_if(libraryList.cbegin(), libraryList.cend(), pred);

  return it != libraryList.cend();
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

  explicit SharedLibraryFinderBDTest(const std::shared_ptr<const AbstractIsExistingValidSharedLibrary> & isExistingValidShLibOp,
                                     QObject *parent = nullptr)
   : AbstractSharedLibraryFinder(isExistingValidShLibOp, parent)
  {
  }

  void setLibraryNamesToNotRedistribute(const std::vector<std::string> & names)
  {
    mLibraryNamesToNotRedistribute = qStringListFromUtf8Strings(names);
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

  bool doLibraryShouldBeDistributed(const QString & libraryName) const noexcept override
  {
    return !mLibraryNamesToNotRedistribute.contains(libraryName);
  }

  QFileInfo doFindLibraryAbsolutePath(const QString & libraryName, const BinaryDependenciesFile & dependentFile) override
  {
    assert( !libraryName.trimmed().isEmpty() );

    QStringList pathList;

    for( const auto & rpathEntry : dependentFile.rPath() ){
      assert( !rpathEntry.isRelative() );
      pathList.append( rpathEntry.path() );
    }
    pathList.append( mSearchPathList.toStringList() );

    for(const QString & directory : pathList){
      QFileInfo libraryFile(directory, libraryName);
      if( validateIsExistingValidSharedLibrary(libraryFile) ){
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

  PathList mSearchPathList;
  QStringList mLibraryNamesToNotRedistribute;
};
