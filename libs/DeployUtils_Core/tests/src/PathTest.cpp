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
#include "PathTest.h"
#include "Mdt/DeployUtils/PathList.h"
#include "Mdt/DeployUtils/SearchPathList.h"
#include "TestUtils.h"
#include <QLatin1String>
#include <QString>
#include <QStringList>
#include <QCoreApplication>
#include <vector>
#include <string>

using namespace Mdt::DeployUtils;


PathList makePathList(const std::vector<std::string> & strList)
{
  return PathList::fromStringList( qStringListFromUtf8Strings(strList) );
}

void PathTest::initTestCase()
{
}

void PathTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void PathTest::pathListStringListTest()
{
  /*
   * Construct from a initializer list
   */
  PathList pl1{QLatin1String("a"),QLatin1String("b")};
  QCOMPARE( pl1.toStringList(), qStringListFromUtf8Strings({"a","b"}) );
  QVERIFY( pl1.containsPath( QLatin1String("a") ) );
  QVERIFY( pl1.containsPath( QLatin1String("b") ) );
  QVERIFY( !pl1.containsPath( QLatin1String("c") ) );
  /*
   * Construct from an QStringList
   */
  auto pl2 = PathList::fromStringList( qStringListFromUtf8Strings({"c","d"}) );
  QCOMPARE( pl2.toStringList(), qStringListFromUtf8Strings({"c","d"}) );
}

void PathTest::pathListAppendTest()
{
  QFETCH(PathList, initialList);
  QFETCH(QString, path);
  QFETCH(PathList, expectedList);

  PathList list = initialList;

  QCOMPARE(list.toStringList(), initialList.toStringList());
  list.appendPath(path);
  QCOMPARE(list.toStringList(), expectedList.toStringList());
}

void PathTest::pathListAppendTest_data()
{
  QTest::addColumn<PathList>("initialList");
  QTest::addColumn<QString>("path");
  QTest::addColumn<PathList>("expectedList");

  PathList initialList;
  PathList expectedList;

  initialList = {};
  expectedList = makePathList({"1"});
  QTest::newRow("{},1,{1}") << initialList << "1" << expectedList;

  initialList = makePathList({"A"});
  expectedList = makePathList({"A","B"});
  QTest::newRow("{A},B,{A,B}") << initialList << "B" << expectedList;

  initialList = makePathList({"A"});
  expectedList = makePathList({"A"});
  QTest::newRow("{A},A,{A}") << initialList << "A" << expectedList;

  initialList = makePathList({"A","B","C"});
  expectedList = makePathList({"A","C","B"});
  QTest::newRow("{A,B,C},B,{A,C,B}") << initialList << "B" << expectedList;
}

void PathTest::pathListAppendPathListTest()
{
  QFETCH(PathList, initialList);
  QFETCH(PathList, pathList);
  QFETCH(PathList, expectedList);

  PathList list = initialList;

  QCOMPARE(list.toStringList(), initialList.toStringList());
  list.appendPathList(pathList);
  QCOMPARE(list.toStringList(), expectedList.toStringList());
}

void PathTest::pathListAppendPathListTest_data()
{
  QTest::addColumn<PathList>("initialList");
  QTest::addColumn<PathList>("pathList");
  QTest::addColumn<PathList>("expectedList");

  QTest::newRow("0")
    << PathList{}
    << PathList{}
    << PathList{};

  QTest::newRow("1")
    << makePathList({"A"})
    << PathList{}
    << makePathList({"A"});

  QTest::newRow("2")
    << makePathList({"A"})
    << makePathList({""})
    << makePathList({"A"});

  QTest::newRow("3")
    << PathList{}
    << makePathList({"A"})
    << makePathList({"A"});

  QTest::newRow("4")
    << makePathList({"A"})
    << makePathList({"A"})
    << makePathList({"A"});

  QTest::newRow("5")
    << makePathList({"A"})
    << makePathList({"B"})
    << makePathList({"A","B"});

  QTest::newRow("6")
    << makePathList({"A"})
    << makePathList({"B","C"})
    << makePathList({"A","B","C"});
}

void PathTest::pathListPrependTest()
{
  QFETCH(PathList, initialList);
  QFETCH(QString, path);
  QFETCH(PathList, expectedList);

  PathList list = initialList;

  QCOMPARE(list.toStringList(), initialList.toStringList());
  list.prependPath(path);
  QCOMPARE(list.toStringList(), expectedList.toStringList());
}

void PathTest::pathListPrependTest_data()
{
  QTest::addColumn<PathList>("initialList");
  QTest::addColumn<QString>("path");
  QTest::addColumn<PathList>("expectedList");

  PathList initialList;
  PathList expectedList;

  initialList = {};
  expectedList = makePathList({"1"});
  QTest::newRow("{},1,{1}") << initialList << "1" << expectedList;

  initialList = makePathList({"A"});
  expectedList = makePathList({"B","A"});
  QTest::newRow("{A},B,{B,A}") << initialList << "B" << expectedList;

  initialList = makePathList({"A"});
  expectedList = makePathList({"A"});
  QTest::newRow("{A},A,{A}") << initialList << "A" << expectedList;

  initialList = makePathList({"A","B","C"});
  expectedList = makePathList({"B","A","C"});
  QTest::newRow("{A,B,C},B,{B,A,C}") << initialList << "B" << expectedList;
}

void PathTest::pathListPrependPathListTest()
{
  QFETCH(PathList, initialList);
  QFETCH(PathList, pathList);
  QFETCH(PathList, expectedList);

  PathList list = initialList;

  QCOMPARE(list.toStringList(), initialList.toStringList());
  list.prependPathList(pathList);
  QCOMPARE(list.toStringList(), expectedList.toStringList());
}

void PathTest::pathListPrependPathListTest_data()
{
  QTest::addColumn<PathList>("initialList");
  QTest::addColumn<PathList>("pathList");
  QTest::addColumn<PathList>("expectedList");

  QTest::newRow("0")
    << PathList{}
    << PathList{}
    << PathList{};

  QTest::newRow("1")
    << makePathList({"A"})
    << PathList{}
    << makePathList({"A"});

  QTest::newRow("2")
    << makePathList({"A"})
    << makePathList({""})
    << makePathList({"A"});

  QTest::newRow("3")
    << PathList{}
    << makePathList({"A"})
    << makePathList({"A"});

  QTest::newRow("4")
    << makePathList({"A"})
    << makePathList({"A"})
    << makePathList({"A"});

  QTest::newRow("5")
    << makePathList({"A"})
    << makePathList({"B"})
    << makePathList({"B","A"});

  QTest::newRow("6")
    << makePathList({"A"})
    << makePathList({"B","C"})
    << makePathList({"B","C","A"});
}

void PathTest::searchPathListTest()
{
  /*
   * Initial state
   */
  SearchPathList spl;
  QCOMPARE(spl.toStringList(), QStringList({}));
  /*
   * Set a path prefix
   */
  spl.setIncludePathPrefixes(true);
  spl.setPathPrefixList( makePathList({"/opt/liba"}) );
  QCOMPARE(spl.toStringList(), qStringListFromUtf8Strings({"/opt/liba"}));
  /*
   * Set a path suffix
   */
  spl.setPathSuffixList( qStringListFromUtf8Strings({"bin"}) );
  QCOMPARE(spl.toStringList(), qStringListFromUtf8Strings({"/opt/liba","/opt/liba/bin"}));
  /*
   * Prepend a path
   */
  spl.prependPath( QLatin1String("/opt/appc") );
  QCOMPARE(spl.toStringList(), qStringListFromUtf8Strings({"/opt/appc","/opt/liba","/opt/liba/bin"}));
  /*
   * Append a path
   */
  spl.appendPath( QLatin1String("/opt/appd") );
  QCOMPARE(spl.toStringList(), qStringListFromUtf8Strings({"/opt/appc","/opt/liba","/opt/liba/bin","/opt/appd"}));
  /*
   * Specify to not include path prefixes
   */
  spl.setIncludePathPrefixes(false);
  QCOMPARE(spl.toStringList(), qStringListFromUtf8Strings({"/opt/appc","/opt/liba/bin","/opt/appd"}));
  /*
   * Clear
   */
  spl.clear();
  spl.setIncludePathPrefixes(true);
  QCOMPARE(spl.toStringList(), QStringList({}));
  /*
   * Set a list of path prefixes
   */
  spl.setPathPrefixList( makePathList({"/opt/liba","/opt/libb"}) );
  QCOMPARE(spl.toStringList(), qStringListFromUtf8Strings({"/opt/liba","/opt/libb"}));
  /*
   * Prepend a path
   */
  spl.prependPath( QLatin1String("/opt/appc") );
  QCOMPARE(spl.toStringList(), qStringListFromUtf8Strings({"/opt/appc","/opt/liba","/opt/libb"}));
  /*
   * Set a list of path suffixes
   */
  spl.setPathSuffixList( qStringListFromUtf8Strings({"bin","lib"}) );
  QCOMPARE(spl.toStringList(), qStringListFromUtf8Strings({"/opt/appc","/opt/liba","/opt/liba/bin","/opt/liba/lib","/opt/libb","/opt/libb/bin","/opt/libb/lib"}));
  /*
   * Clear
   * - By default, path prefixes are not included
   */
  spl.clear();
  QCOMPARE(spl.toStringList(), QStringList({}));
  /*
   * Set a list of path prefixes
   */
  spl.setPathPrefixList( makePathList({"/opt/liba","/opt/libb"}) );
  QCOMPARE(spl.toStringList(), qStringListFromUtf8Strings({}));
  /*
   * Set a list of path suffixes
   */
  spl.setPathSuffixList( qStringListFromUtf8Strings({"bin","lib"}) );
  QCOMPARE(spl.toStringList(), qStringListFromUtf8Strings({"/opt/liba/bin","/opt/liba/lib","/opt/libb/bin","/opt/libb/lib"}));
  /*
   * Setup list of path prefixes and suffixes
   * (This is how QtLibrary uses it to find plugins)
   */
  spl.clear();
  spl.setIncludePathPrefixes(true);
  spl.setPathPrefixList( makePathList({"/opt/tools/bin","/opt/qt/bin"}) );
  spl.setPathSuffixList( qStringListFromUtf8Strings({"qt5",".."}) );
  QCOMPARE(spl.toStringList(), qStringListFromUtf8Strings({"/opt/tools/bin","/opt/tools/bin/qt5","/opt/tools","/opt/qt/bin","/opt/qt/bin/qt5","/opt/qt"}));
  /*
   * When using .. suffix, check that order is preserved
   * (Bug discovered 20171101)
   */
  spl.clear();
  spl.setIncludePathPrefixes(true);
  spl.setPathPrefixList( makePathList({"/opt/liba/bin","/opt/libb","/opt/liba/lib"}) );
  spl.setPathSuffixList( qStringListFromUtf8Strings({".."}) );
  QCOMPARE(spl.toStringList(), qStringListFromUtf8Strings({"/opt/liba/bin","/opt/liba","/opt/libb","/opt","/opt/liba/lib"}));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  PathTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
