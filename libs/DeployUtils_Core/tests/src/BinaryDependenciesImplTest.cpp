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
#include "catch2/catch.hpp"
#include "Catch2QString.h"
#include "BinaryDependenciesTestCommon.h"
#include "TestUtils.h"
#include "Mdt/DeployUtils/Impl/BinaryDependencies.h"
#include <QFileInfo>
#include <QDir>
#include <QMap>
#include <string>
#include <iostream>
#include <algorithm>

using namespace Mdt::DeployUtils;
using Impl::ExecutableFileInfo;
using Impl::ExecutableFileInfoList;

ExecutableFileInfo executableFileInfoFromFullPath(const std::string & filePath)
{
  ExecutableFileInfo efi;
  QFileInfo fi( QString::fromStdString(filePath) );
  assert( !fi.filePath().isEmpty() ); // see doc of QFileInfo::absoluteFilePath()
  assert( !fi.absoluteFilePath().isEmpty() );
  efi.fileName = fi.fileName();
  efi.directoryPath = fi.absoluteDir().path();

  return efi;
}

bool dependenciesEquals(const ExecutableFileInfoList & efis, const std::vector<std::string> & list)
{
  if( efis.size() != list.size() ){
    std::cerr << "dependencies list differs in size. actual: " << efis.size() << ", expected: " << list.size() << std::endl;
    return false;
  }
  for(size_t i = 0; i < efis.size(); ++i){
    const std::string actualFilePath = efis.at(i).toFileInfo().absoluteFilePath().toStdString();
    if( actualFilePath != list.at(i) ){
      std::cerr << "dependencies differs at index " << i << ":";
      std::cerr << "\nactual  : " << actualFilePath;
      std::cerr << "\nexpected: " << list.at(i) << std::endl;
      return false;
    }
  }

  return true;
}

bool pathListContains(const PathList & pathList, const std::string & path)
{
  return pathList.containsPath( QString::fromStdString(path) );
}

void debugOutPathList(const PathList & pathList)
{
  std::cout << pathList.toStringList().join(QLatin1Char('\n')).toStdString() << std::endl;
}

/*
 * A simplified tree to test dependencies
 * This is a basic limited implementation of a tree,
 * to avoid creating a tree class just for this test
 *
 * TODO: could use somthing other than QMap,
 * because we finally allways iterate over it.
 * Also, private methods are poorly named.
 */
class TestDependencyTree
{
 public:

  void setDirectDependencies(const std::vector<std::string> & dependencies)
  {
    mIsAtLeef = false;
    mDependencies.clear();
    mCurrentDirectDependency.clear();

    for(const auto & dependency : dependencies){
      assert( isFileWithoutPath(dependency) );
      mDependencies.insert( QString::fromStdString(dependency), QStringList() );
    }
  }

  void addDependenciesToDirectDependency(const std::string & directDependency, const std::vector<std::string> & dependencies)
  {
    assert( mDependencies.contains( QString::fromStdString(directDependency) ) );

    QStringList qStringDependencies;
    for(const auto & dependency : dependencies){
      assert( isFileWithoutPath(dependency) );
      qStringDependencies.push_back( QString::fromStdString(dependency) );
    }
    mDependencies[QString::fromStdString(directDependency)] = qStringDependencies;
  }

  void setCurrentFile(const QString & file)
  {
    assert( isFileWithoutPath(file) );

    if( containsKey(file) ){
      mCurrentDirectDependency = file;
    }else if( !containsValue(file) ){
      mIsAtLeef = false;
    }else{
      mIsAtLeef = true;
    }
  }

  QStringList currentDependencies() const
  {
    if(mIsAtLeef){
      return QStringList();
    }
    if( mCurrentDirectDependency.isEmpty() ){
      return mDependencies.keys();
    }
    return value(mCurrentDirectDependency);
  }

 private:

  using Map = QMap<QString, QStringList>;

  static
  bool stringAreEqualCI(const QString & a, const QString & b) noexcept
  {
    return QString::compare(a, b, Qt::CaseInsensitive) == 0;
  }

  Map::const_iterator iteratorForKey(const QString & key) const noexcept
  {
    Map::const_iterator first = mDependencies.cbegin();
    const Map::const_iterator last = mDependencies.cend();

    while(first != last){
      if( stringAreEqualCI(first.key(), key) ){
        return first;
      }
      ++first;
    }

    return last;
  }

  bool containsKey(const QString & key) const
  {
    return iteratorForKey(key) != mDependencies.cend();
  }

  QStringList value(const QString & key) const
  {
    const auto it = iteratorForKey(key);

    if( it == mDependencies.cend() ){
      return QStringList();
    }

    return it.value();
  }

  bool containsValue(const QString & value) const
  {
    for(const QStringList & deps : mDependencies){
      if( deps.contains(value, Qt::CaseInsensitive) ){
        return true;
      }
    }

    return false;
  }


  static
  bool isFileWithoutPath(const QString & file)
  {
    return QFileInfo(file).fileName() == file;
  }

  static
  bool isFileWithoutPath(const std::string & file)
  {
    return isFileWithoutPath( QString::fromStdString(file) );
  }

  bool mIsAtLeef = false;
  QString mCurrentDirectDependency;
  QMap<QString, QStringList> mDependencies;
};

TEST_CASE("TestDependencyTree")
{
  TestDependencyTree tree;

  /*
   * target
   *   |-A
   */
  SECTION("A")
  {
    tree.setDirectDependencies({"A"});

    SECTION("When setting current file to a value that not exists in the tree, current dependencies will be those from root")
    {
      tree.setCurrentFile( QLatin1String("target") );
      REQUIRE( tree.currentDependencies() == QStringList{QLatin1String("A")} );
    }

    SECTION("When setting current file to A, current dependencies will return nothing (leaf)")
    {
      tree.setCurrentFile( QLatin1String("A") );
      REQUIRE( tree.currentDependencies().isEmpty() );
    }
  }

  /*
   * target
   *   |-A
   *   |-B
   */
  SECTION("A,B")
  {
    tree.setDirectDependencies({"A","B"});

    SECTION("When setting current file to a value that not exists in the tree, current dependencies will be those from root")
    {
      tree.setCurrentFile( QLatin1String("target") );
      REQUIRE( tree.currentDependencies() == QStringList{QLatin1String("A"),QLatin1String("B")} );
    }

    SECTION("When setting current file to A, current dependencies will return nothing (leaf)")
    {
      tree.setCurrentFile( QLatin1String("A") );
      REQUIRE( tree.currentDependencies().isEmpty() );
    }

    SECTION("When setting current file to B, current dependencies will return nothing (leaf)")
    {
      tree.setCurrentFile( QLatin1String("B") );
      REQUIRE( tree.currentDependencies().isEmpty() );
    }
  }

  /*
   * target
   *   |-A
   *     |-B
   */
  SECTION("A which depends on B")
  {
    tree.setDirectDependencies({"A"});
    tree.addDependenciesToDirectDependency("A", {"B"});

    SECTION("When setting current file to a value that not exists in the tree, current dependencies will be those from root")
    {
      SECTION("same case")
      {
        tree.setCurrentFile( QLatin1String("target") );
        REQUIRE( tree.currentDependencies() == QStringList{QLatin1String("A")} );
      }

      SECTION("UPPER case")
      {
        tree.setCurrentFile( QLatin1String("TARGET") );
        REQUIRE( tree.currentDependencies() == QStringList{QLatin1String("A")} );
      }
    }

    SECTION("When setting current file to A, current dependencies will return B")
    {
      SECTION("same case")
      {
        tree.setCurrentFile( QLatin1String("A") );
        REQUIRE( tree.currentDependencies() == QStringList{QLatin1String("B")} );
      }

      SECTION("lower case")
      {
        tree.setCurrentFile( QLatin1String("a") );
        REQUIRE( tree.currentDependencies() == QStringList{QLatin1String("B")} );
      }
    }

    SECTION("When setting current file to B, current dependencies will return nothing (leaf)")
    {
      SECTION("same case")
      {
        tree.setCurrentFile( QLatin1String("B") );
        REQUIRE( tree.currentDependencies().isEmpty() );
      }

      SECTION("lower case")
      {
        tree.setCurrentFile( QLatin1String("b") );
        REQUIRE( tree.currentDependencies().isEmpty() );
      }
    }
  }
}

class TestExecutableFileReader
{
 public:

  void openFile(const QFileInfo & fileInfo)
  {
    mDependencies.setCurrentFile( fileInfo.fileName() );
    mIsOpen = true;
  }

  void openFile(const QFileInfo & fileInfo, const Platform &)
  {
    openFile(fileInfo);
  }

  bool isOpen() const noexcept
  {
    return mIsOpen;
  }

  void close()
  {
    mIsOpen = false;
  }

//   Platform getFilePlatform()
//   {
//   }

  bool isExecutableOrSharedLibrary()
  {
    return true;
  }

  QStringList getNeededSharedLibraries()
  {
    return mDependencies.currentDependencies();
  }

  QStringList getRunPath()
  {
    return mRunPath;
  }

  void setDirectDependencies(const std::vector<std::string> & dependencies)
  {
    mDependencies.setDirectDependencies(dependencies);
  }

  void addDependenciesToDirectDependency(const std::string & directDependency, const std::vector<std::string> & dependencies)
  {
    mDependencies.addDependenciesToDirectDependency(directDependency, dependencies);
  }

  void setRunPath(const QStringList & paths)
  {
    mRunPath = paths;
  }

 private:

  bool mIsOpen = false;
  TestDependencyTree mDependencies;
  QStringList mRunPath;
};


TEST_CASE("ExecutableFileInfo")
{
  ExecutableFileInfo efi;

  SECTION("default constructed")
  {
    REQUIRE( !efi.hasAbsoluteFilePath() );
  }

  SECTION("only file name")
  {
    efi.fileName = QLatin1String("libm.so");
    REQUIRE( !efi.hasAbsoluteFilePath() );
  }

  SECTION("full path")
  {
    efi.fileName = QLatin1String("libm.so");
    efi.directoryPath = QLatin1String("/tmp");
    REQUIRE( efi.hasAbsoluteFilePath() );
  }
}

TEST_CASE("compareExecutableFileInfo")
{
  using Impl::compareExecutableFileInfo;

  ExecutableFileInfo a, b;

  SECTION("a:/a - b:/a")
  {
    a.fileName = QLatin1String("a");
    a.directoryPath = QLatin1String("/");
    b.fileName = QLatin1String("a");
    b.directoryPath = QLatin1String("/");

    REQUIRE( !compareExecutableFileInfo(a, b) );
  }

  SECTION("a:/a - b:/b")
  {
    a.fileName = QLatin1String("a");
    a.directoryPath = QLatin1String("/");
    b.fileName = QLatin1String("b");
    b.directoryPath = QLatin1String("/");

    REQUIRE( compareExecutableFileInfo(a, b) );
  }
}

TEST_CASE("executableFileInfoAreEqual")
{
  using Impl::executableFileInfoAreEqual;

  ExecutableFileInfo a, b;

  SECTION("a:/a - b:/a")
  {
    a.fileName = QLatin1String("a");
    a.directoryPath = QLatin1String("/");
    b.fileName = QLatin1String("a");
    b.directoryPath = QLatin1String("/");

    REQUIRE( executableFileInfoAreEqual(a, b) );
  }

  SECTION("a:/a - b:/b")
  {
    a.fileName = QLatin1String("a");
    a.directoryPath = QLatin1String("/");
    b.fileName = QLatin1String("b");
    b.directoryPath = QLatin1String("/");

    REQUIRE( !executableFileInfoAreEqual(a, b) );
  }
}

TEST_CASE("findLibraryAbsolutePath")
{
  using Impl::findLibraryAbsolutePath;

  PathList pathList;
  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  ExecutableFileInfo library;
  const auto platform = Platform::nativePlatform();

  SECTION("libA.so - pathList:/tmp - exists")
  {
    pathList.appendPath( QLatin1String("/tmp") );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/libA.so"});
    library = findLibraryAbsolutePath( QLatin1String("libA.so"), pathList, platform, isExistingSharedLibraryOp );
    REQUIRE( library.hasAbsoluteFilePath() );
    REQUIRE( library.toFileInfo().absoluteFilePath() == QLatin1String("/tmp/libA.so") );
  }

  SECTION("libA.so - pathList:/tmp,/opt - exists in both path - must pick the first one")
  {
    pathList.appendPathList( {QLatin1String("/tmp"),QLatin1String("/opt")} );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/libA.so","/opt/libA.so"});
    library = findLibraryAbsolutePath( QLatin1String("libA.so"), pathList, platform, isExistingSharedLibraryOp );
    REQUIRE( library.hasAbsoluteFilePath() );
    REQUIRE( library.toFileInfo().absoluteFilePath() == QLatin1String("/tmp/libA.so") );
  }
}

TEST_CASE("makeDirectoryFromRpath")
{
  using Impl::makeDirectoryFromRpath;

  ExecutableFileInfo originExecutable;
  originExecutable.fileName = QLatin1String("myapp");
  originExecutable.directoryPath = QLatin1String("/opt");

  SECTION("rpath item:/tmp")
  {
    REQUIRE( makeDirectoryFromRpath( originExecutable, QLatin1String("/tmp") ) == QLatin1String("/tmp") );
  }

  SECTION("rpath item:$ORIGIN")
  {
    REQUIRE( makeDirectoryFromRpath( originExecutable, QLatin1String("$ORIGIN") ) == QLatin1String("/opt") );
  }

  SECTION("rpath item:$ORIGIN/../lib")
  {
    REQUIRE( makeDirectoryFromRpath( originExecutable, QLatin1String("$ORIGIN/../lib") ) == QLatin1String("/lib") );
  }
}

TEST_CASE("findLibraryAbsolutePathByRpath")
{
  using Impl::findLibraryAbsolutePathByRpath;

  QStringList rpath;
  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  ExecutableFileInfo library;

  ExecutableFileInfo originExecutable;
  originExecutable.fileName = QLatin1String("myapp");
  originExecutable.directoryPath = QLatin1String("/opt");

  SECTION("libA.so - rpath:/tmp - exists")
  {
    rpath = qStringListFromUtf8Strings({"/tmp"});
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/libA.so"});
    library = findLibraryAbsolutePathByRpath(originExecutable, QLatin1String("libA.so"), rpath, isExistingSharedLibraryOp);
    REQUIRE( library.hasAbsoluteFilePath() );
    REQUIRE( library.toFileInfo().absoluteFilePath() == QLatin1String("/tmp/libA.so") );
  }

  SECTION("libA.so - rpath:$ORIGIN - exists")
  {
    rpath = qStringListFromUtf8Strings({"$ORIGIN"});
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/opt/libA.so"});
    library = findLibraryAbsolutePathByRpath(originExecutable, QLatin1String("libA.so"), rpath, isExistingSharedLibraryOp);
    REQUIRE( library.hasAbsoluteFilePath() );
    REQUIRE( library.toFileInfo().absoluteFilePath() == QLatin1String("/opt/libA.so") );
  }

  SECTION("libA.so - rpath:/tmp - not exists")
  {
    rpath = qStringListFromUtf8Strings({"/tmp"});
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/lib/libA.so"});
    library = findLibraryAbsolutePathByRpath(originExecutable, QLatin1String("libA.so"), rpath, isExistingSharedLibraryOp);
    REQUIRE( library.isNull() );
  }
}

TEST_CASE("findDependencies")
{
//   using Impl::findDependencies;

  Impl::FindDependenciesImpl impl;
  ExecutableFileInfo target;
  PathList searchPathList;
  TestExecutableFileReader reader;
  const auto platform = Platform::nativePlatform();
  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  ExecutableFileInfoList dependencies;

//   qDebug() << "----------------";

  SECTION("no dependencies")
  {
    target = executableFileInfoFromFullPath("/tmp/libm.so");
//     debugExecutableFileInfo(target);
    impl.findDependencies(target, dependencies, searchPathList, reader, platform, isExistingSharedLibraryOp);
//     debugExecutableFileInfoList(dependencies);
    REQUIRE( dependencies.size() == 0 );
  }

  /*
   * Target: /tmp/build/myapp
   * Dependencies:
   *  /opt/MyLibs/libMyLibA.so
   */
  SECTION("myapp depends on MyLibA")
  {
    target = executableFileInfoFromFullPath("/tmp/myapp");
    searchPathList.appendPath( QLatin1String("/opt/MyLibs/") );
    reader.setDirectDependencies({"libMyLibA.so"});
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/opt/MyLibs/libMyLibA.so"});

    impl.findDependencies(target, dependencies, searchPathList, reader, platform, isExistingSharedLibraryOp);

    REQUIRE( dependencies.size() == 1 );
    REQUIRE( dependenciesEquals(dependencies, {"/opt/MyLibs/libMyLibA.so"}) );
  }

  /*
   * Target: /tmp/build/myapp
   * Dependencies:
   *  /opt/MyLibs/libMyLibA.so
   *    |-dependencies:
   *      /opt/Qt/libQt5Core.so
   */
  SECTION("myapp depends on MyLibA which depends on Qt5Core")
  {
    target = executableFileInfoFromFullPath("/tmp/myapp");
    searchPathList.appendPathList( {QLatin1String("/opt/MyLibs/"),QLatin1String("/opt/qt/")} );
    reader.setDirectDependencies({"libMyLibA.so"});
    reader.addDependenciesToDirectDependency("libMyLibA.so",{"libQt5Core.so"});
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/opt/MyLibs/libMyLibA.so","/opt/qt/libQt5Core.so"});

    impl.findDependencies(target, dependencies, searchPathList, reader, platform, isExistingSharedLibraryOp);

    REQUIRE( dependencies.size() == 2 );
    REQUIRE( dependenciesEquals(dependencies, {"/opt/MyLibs/libMyLibA.so","/opt/qt/libQt5Core.so"}) );
  }

  /*
   * Target: /tmp/build/myapp
   * Dependencies:
   *  /opt/MyLibs/libMyLibA.so
   *    |-dependencies:
   *      /opt/Qt/libQt5Core.so
   *  /opt/MyLibs/libMyLibB.so
   *    |-dependencies:
   *      /opt/Qt/libQt5Core.so
   */

}
